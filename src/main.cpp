#ifndef UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include "RemoteClient.h"

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

    Serial.println("Connected to the WiFi network");

    int e = ssl_connect(IPAddress(192, 168, 2, 22), 6467);
    Serial.println(e);

    ssl_send("Hello, world!", 13);
}

void loop() {

    char buffer[80];
    int sz = ssl_read(buffer, sizeof(buffer) - 1);
    if (sz > 0) {
        for (int i = 0; i < sz; i++) {
            Serial.print(buffer[i]);
        }
    }
}
#endif