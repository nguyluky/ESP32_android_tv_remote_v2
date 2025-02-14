#include <Arduino.h>
#include <ESPmDNS.h>
void printPacket(uint8_t *packet, size_t len);
IPAddress findTVIp(String macAddress);
