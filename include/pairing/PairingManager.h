#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Crypto.h>
// #include <SHA256.h>
#include <vector>
#include "pairing/PairingMessageManager.h"
#include "certificate/CertificateGenerator.h"
#include "RemoteClient.h"

#ifndef PAIRINGMANAGER_H
#define PAIRINGMANAGER_H
class PairingManager {
public:
    bool sendCode(const String& code);
    void begin(IPAddress host, uint16_t port, char* service_name);
    bool connected();
    void loop();
    bool isSecure = false;

private:
    std::vector<uint8_t> hexStringToBytes(const String &hexString);
    void handleResponse(Pairing__PairingMessage *message);
    std::vector<uint8_t> chunks;
};
#endif