#include "pairing/PairingMessageManager.h"

PairingMessageManager::PairingMessageManager() {
    manufacturer = "ESP32";
    model = "Generic";
}

uint8_t *PairingMessageManager::encodePairingMessage(Pairing__PairingMessage &message) {
    // the firt number is the size of the buffer
    size_t bufferSize = pairing__pairing_message__get_packed_size(&message);
    if (bufferSize == 0) {
        Serial.println("[ERROR]: Buffer size is 0");
        return NULL;
    }

    uint8_t *buffer = (uint8_t *)malloc(bufferSize + 1);
    if (buffer == NULL) {
        Serial.println("[ERROR]: Memory allocation failed");
        return NULL;
    }

    buffer[0] = bufferSize;

    size_t packedSize = pairing__pairing_message__pack(&message, buffer+1);
    if (packedSize != bufferSize) {
        Serial.println("[ERROR]: Packing failed");
        free(buffer);
        return NULL;
    }
    return buffer;
}

// remember to free the buffer after using it
uint8_t *PairingMessageManager::createPairingRequest(char *service_name) {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingRequest request = PAIRING__PAIRING_REQUEST__INIT;
    message.pairing_request = &request;

    message.pairing_request->service_name = (char *)service_name;
    message.pairing_request->client_name = model;

    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;

    return encodePairingMessage(message); 
}

uint8_t* PairingMessageManager::createPairingOption() {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
        Pairing__PairingOption option = PAIRING__PAIRING_OPTION__INIT;
        option.preferred_role = PAIRING__ROLE_TYPE__ROLE_TYPE_INPUT;
            Pairing__PairingEncoding encodings[] = {PAIRING__PAIRING_ENCODING__INIT};
            encodings[0].type = PAIRING__PAIRING_ENCODING__ENCODING_TYPE__ENCODING_TYPE_HEXADECIMAL;
            encodings[0].symbol_length = 6;
        Pairing__PairingEncoding *array = encodings;
        option.input_encodings = &array;
        option.n_input_encodings = 1;


    message.pairing_option = &option;
    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;

    uint8_t *buffer = encodePairingMessage(message);
    return buffer;
}

uint8_t* PairingMessageManager::createPairingConfiguration() {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingConfiguration configuration = PAIRING__PAIRING_CONFIGURATION__INIT;
    message.pairing_configuration = &configuration;

    message.pairing_configuration->client_role = PAIRING__ROLE_TYPE__ROLE_TYPE_INPUT;
    Pairing__PairingEncoding encoding = PAIRING__PAIRING_ENCODING__INIT;
    message.pairing_configuration->encoding = &encoding;

    message.pairing_configuration->encoding->type = PAIRING__PAIRING_ENCODING__ENCODING_TYPE__ENCODING_TYPE_HEXADECIMAL;
    message.pairing_configuration->encoding->symbol_length = 6;

    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;
    return encodePairingMessage(message);
}

uint8_t *PairingMessageManager::createPairingSecret(const uint8_t *secret) {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingSecret secretMessage = PAIRING__PAIRING_SECRET__INIT;
    message.pairing_secret = &secretMessage;

    message.pairing_secret->secret.data = (uint8_t *)secret;
    message.pairing_secret->secret.len = 32;

    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;
    return encodePairingMessage(message);
}