
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
    PairingMessageManager();
    uint8_t* createPairingRequest(char *service_name);
    uint8_t* createPairingOption() ;
    uint8_t* createPairingConfiguration() ;
    uint8_t* createPairingSecret(const uint8_t *secret) ;
    uint8_t* encodePairingMessage(Pairing__PairingMessage &message);
};

extern PairingMessageManager pairingMessageManager;
#endif //PAIRING_MESSAGE_MANAGER_H