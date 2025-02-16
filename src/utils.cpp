#include "utils.h"
#include <ESPmDNS.h>

void printPacket(uint8_t *packet, size_t len) {
    Serial.print("unit8array: [");
    for (size_t i = 0; i < len; i++) {
        Serial.printf("0x%02X", packet[i]);
        if (i < len - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");

    Serial.print("hex: ");
    for (size_t i = 0; i < len; i++) {
        Serial.printf("%02X", packet[i]);
    }
    Serial.println();

}

IPAddress findTVIp(String macAddress) {
    MDNS.begin("esp32");
    bool found = false;
    IPAddress ip;

    while (!found)
    {
        int len = MDNS.queryService("androidtvremote2", "tcp");
        for (int i = 0; i < len; i++) {
            Serial.print("host_name = ");
            Serial.print(MDNS.hostname(i));
            ip = MDNS.IP(i);
            Serial.print(",IP = ");
            Serial.print(ip.toString());
            if (MDNS.hasTxt(i, "bt")) {
                String mc = MDNS.txt(i, "bt");
                Serial.print(",mac_address = ");
                Serial.print(mc);

                if (mc == macAddress) {
                    found = true;
                    break;
                }
            }
            Serial.println();
        }
        delay(1000);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    Serial.println();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    Serial.printf("Found TV IP: %s\n", ip.toString());
    MDNS.end();
    return ip;
}