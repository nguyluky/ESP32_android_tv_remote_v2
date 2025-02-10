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


    if (Serial.available() > 0) {
        String command = Serial.readString();
        Serial.println(command);
        if (command == "power") {
            remoteManager.sendPower();
        } else if (command == "up") {
            remoteManager.sendKey(REMOTE__REMOTE_KEY_CODE__KEYCODE_DPAD_UP, REMOTE__REMOTE_DIRECTION__SHORT);
        } else if (command == "down") {
            remoteManager.sendKey(REMOTE__REMOTE_KEY_CODE__KEYCODE_DPAD_DOWN, REMOTE__REMOTE_DIRECTION__SHORT);
        } else if (command == "left") {
            remoteManager.sendKey(REMOTE__REMOTE_KEY_CODE__KEYCODE_DPAD_LEFT, REMOTE__REMOTE_DIRECTION__SHORT);
        } else if (command == "right") {
            remoteManager.sendKey(REMOTE__REMOTE_KEY_CODE__KEYCODE_DPAD_RIGHT, REMOTE__REMOTE_DIRECTION__SHORT);
        } else if (command == "ok") {
            remoteManager.sendKey(REMOTE__REMOTE_KEY_CODE__KEYCODE_DPAD_CENTER, REMOTE__REMOTE_DIRECTION__SHORT);
        } 
    }
    delay(100);
}
#endif