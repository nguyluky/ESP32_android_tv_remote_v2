#ifndef UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include "pairing/PairingManager.h"

const char* ssid = "SieuCoi";
const char* password = "0902838500";

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, world!");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    PairingManager pairingManager;
    pairingManager.begin(IPAddress(192, 168, 2, 22), 6467, "service_name");

    while (true) {
        pairingManager.loop();
    }


    Serial.println("[INFO]: DONE");

}

void loop() {

}
#endif