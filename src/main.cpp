#include "../include/esp8266/Arduino.h"
#include "../include/libraries/ESP8266WiFi/src/WiFiClient.h"
#include "../include/libraries/ESP8266WiFi/src/ESP8266WiFi.h"
#include "../.pio/libdeps/esp01_1m/OneWire/OneWire.h"
#include "../.pio/libdeps/esp01_1m/DallasTemperature/DallasTemperature.h"
#include "../include/esp8266/wl_definitions.h"
#include "../.pio/libdeps/esp01_1m/ESP_EEPROM/src/ESP_EEPROM.h"

const char* ssid2 = "RIZKI";
const char* pass2 = "strawberry256";

const int oneWireBus = 2;

WiFiClient client;

OneWire oneWire(oneWireBus);
DallasTemperature tempSens(&oneWire);
DeviceAddress tempDev;

class credentials2{
public:
    char* ssid;
    char* pass;
//
//
    credentials2(){}
    credentials2(String ssid, String pass){
        int s = ssid.length();
        char* buf = new char [s];
        ssid.toCharArray(buf, s+1);
        this->ssid = buf;

        s = pass.length();
        Serial.println(ssid);
        Serial.println(pass);
        Serial.println(buf);
        buf = new char [s];
        pass.toCharArray(buf, s+1);
        this->pass = buf;
        Serial.println(buf);
    }
//
};




void setup() {
//    Serial.begin(115200);
//    while(!Serial);
//    EEPROM.begin(32); // looks like 16 bytes is the minimum
//    EEPROM.put(0, 1234); // first parameter sets the position in the buffer, second the value
//    EEPROM.put(4, 4567); // first parameter sets the position in the buffer, second the value
//    boolean res = EEPROM.commit();
//    Serial.println(res); // should print 1 (true) if commit worked as expected
//    int myVar;
//    int myVar2;
//    EEPROM.get(0, myVar);
//    EEPROM.get(4, myVar2);
//    Serial.println(myVar);
//    Serial.println(myVar2);
    char* ssid;
    char* pass;
    int ssidlen;
    EEPROM.begin(32);
    Serial.begin(115200);
    pinMode(oneWireBus, INPUT);

    delay(1000);
    Serial.println();

    Serial.println();
    Serial.println("SSID before: ");
    Serial.println("pass before: ");
    Serial.println();

    pinMode(oneWireBus, INPUT);

    delay(1000);

    if(true){
//        creden = credentials2("RIZKI", "strawberry256");
        Serial.println("Writing...");
        String ssid = "RIZKI";
        char* buf = new char[ssid.length()];
        ssid.toCharArray(buf, ssid.length()+1);
        Serial.println("Loop");
        for (int i = 0; i < ssid.length(); ++i) {
            char a = EEPROM.read(0+i);
            Serial.println(a);
        }
        Serial.println("Loop End");
        EEPROM.put(0, buf);
        Serial.println("buf1");
        Serial.println(buf);
        String pass = "strawberry256";
        buf = new char[pass.length()];
        pass.toCharArray(buf, pass.length()+1);
        EEPROM.put(ssid.length()+1, buf);
        Serial.println("buf2");
        Serial.println(buf);
        delay(500);
        int res = EEPROM.commit();
        Serial.println(res);
        ssidlen = ssid.length();
    }
    EEPROM.get(0, ssid);
    delay(100);
    EEPROM.get(ssidlen+1, pass);
    delay(100);
    Serial.println("Connecting to ssid: ");
    delay(100);
    Serial.print(ssid);
    Serial.println("Pass: ");
    delay(100);
    Serial.print(pass);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED){
        Serial.print("--");
        delay(1000);
    }
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
//    tempSens.requestTemperatures();
//    float tempC = tempSens.getTempCByIndex(0);
//    Serial.println(tempC);
//    Serial.println(" Temp");
//    delay(6000);
}

