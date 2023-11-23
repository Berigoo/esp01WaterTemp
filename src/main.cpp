#include "../include/esp8266/Arduino.h"
#include "..//include/libraries/ESP8266WiFi/src/WiFiClient.h"
#include "../include/libraries/ESP8266WiFi/src/ESP8266WiFi.h"
#include "../.pio/libdeps/esp01_1m/OneWire/OneWire.h"
#include "../.pio/libdeps/esp01_1m/DallasTemperature/DallasTemperature.h"
#include "../include/esp8266/wl_definitions.h"
#include "../.pio/libdeps/esp01_1m/ESP_EEPROM/src/ESP_EEPROM.h"

const int oneWireBus = 2;

WiFiClient client;

OneWire oneWire(oneWireBus);
DallasTemperature tempSens(&oneWire);
DeviceAddress tempDev;

class credentials2{
private:
    char ssid[15] = {0};
    char pass[15] = {0};

    size_t s_ssid = 0;
    size_t s_pass = 0;
public:
    credentials2(){}
    credentials2(String ssid, String pass){
        changeCredentials(ssid, pass);
    }

    int writeAndCommit(){
        char buf[50];
        snprintf(buf, 50, "\nCOMMIT\nssid: %s\npass: %s\n", this->ssid, this->pass);
        Serial.println(buf);
        EEPROM.put(0, ssid);
        EEPROM.put(s_ssid+1, pass);         // offset 1 for \0
        return EEPROM.commit();
    }

    void calcLen(){
        this->s_ssid = 0;
        this->s_pass = 0;
        int i = 0;
        while((char)EEPROM.read(i) != '\0'){
            s_ssid++;
            i++;
        }
        i = 0;
        while((char)EEPROM.read(i+s_ssid+1) != '\0'){
            s_pass++;
            i++;
        }
    }

    char* getSsid(){
        if (this->ssid[0] == '\0') return NULL;
        else return ssid;
    }

    char* getPass(){
        if (this->pass[0] == '\0') return NULL;
        else return pass;
    }

    void changeCredentials(String ssid, String pass){
        reset();

        s_ssid = ssid.length();
        s_pass = pass.length();

        char* buf = &this->ssid[0];
        ssid.toCharArray(buf, s_ssid+1);
        buf[s_ssid] = '\0';

        buf = &this->pass[0];
        pass.toCharArray(buf, s_pass+1);
        buf[s_pass] = '\0';

        char b[50];
        snprintf(b, 50, "changed into: %s, %s", this->ssid, this->pass);
        Serial.println(b);

    }

    void reset(){
        for (int i = 0 ; i < EEPROM.length() ; i++) {
            EEPROM.write(i, 0);
        }

        EEPROM.commit();
    }

    void EepromRead(){
        Serial.print("\nssid & pass ");
        Serial.print(this->ssid);
        Serial.print(" : ");
        Serial.println(this->pass);
        calcLen();

        for(int i=0; i<s_ssid; i++){
            this->ssid[i] = EEPROM.read(i);
        }
        for(int i=0; i<s_pass; i++){
            this->pass[i] = EEPROM.read(s_ssid+1+i);    // offset 1 for \0
        }
        char buf[80];
        snprintf(buf, 80, "\nGET\nssid len: %i\npass len: %i\nssid: %s\npass: %s\n", s_ssid, s_pass, this->ssid, this->pass);
        Serial.println(buf);
    }

    int connectToWifi(){
        if(WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect(false);
            delay(2000);
        }

        if(this->ssid[0] != '\0'){
            char buf[40];
            snprintf(buf, 40, "\nConnect to: %s, %s", this->ssid, this->pass);
            Serial.println(buf);
            WiFi.begin(this->ssid, this->pass);
            while (WiFi.status() != WL_CONNECTED){
                Serial.print(".");
                delay(1000);
                if(Serial.available()>0){
                    char buf[Serial.available()];
                    int i = 0;
                    while(1){
                        buf[i] = (char)Serial.read();
                        if(Serial.available() <= 0) break;
                        i++;
                    }
                    Serial.println(buf);
                    char* tok = strtok(buf, " ");
                    char* first = tok; first[strlen(first)-2] = '\0'; //removing unwanted char
                    if(strcmp(first, "wifi")){
                        Serial.println("Entering...");
                        tok = strtok(0, " ");
                        char* sec = tok; sec[strlen(sec)] = '\0';
                        String ssid = sec;
                        tok = strtok(0, " ");
                        char* third = tok; third[strlen(third)-6] = '\0';
                        String pass = third;

                        changeCredentials(ssid, pass);
                        writeAndCommit();
                        WiFi.begin(this->ssid, this->pass);
                    }
                }
            }
            Serial.println("Connected!");
            return 1;
        }
        return 0;
    }

    void changeCredentials(String token) {       //handle 'wifi' command
        Serial.println(token);
        int first = token.indexOf(" ");
        int sec = token.indexOf(" ", first+1);

        String ssid = token.substring(first+1, sec);
        String pass = token.substring(sec+1, token.length()-2);

        changeCredentials(ssid, pass);
        writeAndCommit();
        connectToWifi();
    }
};


credentials2 creden;

void setup() {
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    EEPROM.begin(32);
    Serial.begin(115200);
    pinMode(oneWireBus, INPUT);

    creden.EepromRead();

    creden.connectToWifi();

    Serial.println(WiFi.localIP());

    Serial.println("Locating devices....");
    tempSens.begin();
    delay(1000);
    Serial.println("Done");
    Serial.println("Getting device count: ");
    Serial.print(tempSens.getDeviceCount());
    Serial.print(" devices");

    if(!tempSens.getAddress(tempDev, 0)) Serial.println("Unable to find device address");
}

// the loop function runs over and over again forever
void loop() {
    tempSens.requestTemperatures();
    float tempC = tempSens.getTempCByIndex(0);
    Serial.print(tempC);
    Serial.println(" Temp");

    if(Serial.available() > 0){
        char buf[Serial.available()];
        String buff;
        while (Serial.available()>0){
            while(1){
                buff.concat((char)Serial.read());
                if(Serial.available() <= 0) break;
            }
        }
        Serial.print("Retrieved Msg: ");
        Serial.println(buff);
        Serial.println(buff.substring(0, buff.indexOf(" ")));
        Serial.println(buff);
        if(buff.substring(0, buff.indexOf(" ")) == "wifi"){
            Serial.println("Enter");
            creden.changeCredentials(buff);
        }
    }

    delay(6000);
}

