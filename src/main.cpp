#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "certificate/CertificateGenerator.h"

const static char* ssid = "SieuCoi";
const static char* password = "0902838500";


void getMDNS() {
  Serial.println("Getting MDNS");

  if (!MDNS.begin("esp32")) {
    Serial.println("[ERROR] setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  int n = MDNS.queryService("_androidtvremote2", "_tcp");
}

void init_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("[DEBUG]: connecting to wifi ...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("[DEBUG]: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  init_wifi();
}

void loop() {
}