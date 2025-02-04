#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Crypto.h>
#include <SHA256.h>
#include <vector>
#include "pairing/PairingMessageManager.h"
#include "certificate/CertificateGenerator.h"

#ifndef PAIRINGMANAGER_H
#define PAIRINGMANAGER_H
class PairingManager {
public:
    PairingManager(char* host, uint16_t port, char* service_name);

    bool sendCode(const String& code);
    void start();

private:
    std::vector<uint8_t> hexStringToBytes(const String &hexString);
    void handleResponse(Pairing__PairingMessage *message);
    char* host;
    uint16_t port;
    char* service_name;
    WiFiClientSecure client;
    std::vector<uint8_t> chunks;
};
#endif