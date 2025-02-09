#ifndef UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include "pairing/PairingManager.h"
#include "remote/RemoteManager.h" 

const char* ssid = "SieuCoi";
const char* password = "0902838500";

RemoteManager remoteManager;
PairingManager pairingManager;

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, world!");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // PairingManager pairingManager;
    // pairingManager.begin(IPAddress(192, 168, 2, 243), 6467, "service_name");

    // while (pairingManager.connected()) {
    //     pairingManager.loop();
    //     if (pairingManager.isSecure) {
    //         Serial.println("Enter code: ");
    //         if (Serial.available() > 0) {
    //             String code = Serial.readString();
    //             Serial.println(code);
    //             if (code.length() == 6) {
    //                 pairingManager.sendCode(code);
    //             }
    //         }
    //     }
    //     delay(500);
    // }

    remoteManager.start(IPAddress(192, 168, 2, 243), 6466);
    
    Serial.println("[INFO]: start loop");

}

void loop() {
    remoteManager.loop();

    if (remoteManager.error_auth) {
        pairingManager.begin(IPAddress(192, 168, 2, 243), 6467, "service_name");
        while (pairingManager.connected()) {
            pairingManager.loop();
            if (pairingManager.isSecure) {
                Serial.println("Enter code: ");
                if (Serial.available() > 0) {
                    String code = Serial.readString();
                    Serial.println(code);
                    if (code.length() == 6) {
                        pairingManager.sendCode(code);
                    }
                }
            }
            delay(500);
        }
        remoteManager.error_auth = false;

        remoteManager.start(IPAddress(192, 168, 2, 243), 6466);
    }
    delay(100);
}
#endif