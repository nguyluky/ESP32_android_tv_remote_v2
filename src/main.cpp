#ifndef UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include "certificate/CertificateGenerator.h"
#include "pairing/PairingManager.h"
#include "esp_log.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

const static char *ssid = "SieuCoi";
const static char *password = "0902838500";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

WiFiClientSecure client;

// FIX: this function is not working
void getMDNS()
{
    Serial.println("Getting MDNS");

    if (!MDNS.begin("esp32"))
    {
        Serial.println("[ERROR] setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }

    int n = MDNS.queryService("_androidtvremote2", "_tcp");

    if (n == 0)
    {
        Serial.println("[ERROR] no services found");
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            Serial.print("Service ");
            Serial.print(i + 1);
            Serial.print(" found: ");
            Serial.print(MDNS.hostname(i));
            Serial.print(" (");
            Serial.print(MDNS.IP(i));
            Serial.print(":");
            Serial.print(MDNS.port(i));
            Serial.println(")");
        }
    }
}

void init_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("[DEBUG]: connecting to wifi ...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(WiFi.status());
        delay(1000);
    }
    Serial.print("[DEBUG]: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    esp_log_level_set("*", ESP_LOG_DEBUG);
    init_wifi();

    timeClient.begin();
    timeClient.update();
    configTime(0, 0, "pool.ntp.org"); 


    IPAddress ip(192, 168, 2, 139);
    uint16_t port = 6467;
    
    PairingManager pairingManager;
    pairingManager.begin(ip, 6467, "ESP32-Remote" );




    // WiFiClientSecure client;
    // client.setCertificate(client_cert_pem);   // Cài đặt chứng chỉ
    // client.setPrivateKey(client_key_pem);
    // client.setCACert(rootCA);

    // // client.setInsecure(); // Không kiểm tra chứng chỉ

    // Serial.printf("[DEBUG]: %s:%d Pairing started\n", ip.toString(), port);

    // if (!client.connect(ip, port))
    // {
    //     Serial.println("[ERROR]: Connection failed!");
    //     return;
    // }

    // Serial.printf("[DEBUG]: %s Pairing connected\n", ip.toString());

}

void loop()
{
}
#endif