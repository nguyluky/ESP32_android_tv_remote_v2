
#include <Arduino.h>
#include "pairing/pairingmessage.pb-c.h"
#include <pb_encode.h>
#include <pb_decode.h>
#include <pb_common.h>
#include <WiFiClientSecure.h>

#ifndef PAIRING_MESSAGE_MANAGER_H
#define PAIRING_MESSAGE_MANAGER_H

class PairingMessageManager
{
private:
    char *manufacturer;
    char *model;
public:
    PairingMessageManager(/* args */);
    bool sendPairingRequest(WiFiClientSecure &client, const char *service_name);
    bool sendPairingOption(WiFiClientSecure &client) ;
    bool sendPairingConfiguration(WiFiClientSecure &client) ;
    bool sendPairingSecret(WiFiClientSecure &client, const uint8_t *secret) ;

};

extern PairingMessageManager pairingMessageManager;
#endif //PAIRING_MESSAGE_MANAGER_H