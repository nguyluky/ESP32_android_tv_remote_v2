#include "pairing/PairingMessageManager.h"

PairingMessageManager::PairingMessageManager() {
    manufacturer = "ESP32";
    model = "Generic";
}

bool send(WiFiClientSecure &client, Pairing__PairingMessage &message) {
    size_t bufferSize = pairing__pairing_message__get_packed_size(&message);
    if (bufferSize == 0) {
        Serial.println("[ERROR]: Buffer size is 0");
        return false;
    }

    uint8_t *buffer = (uint8_t *)malloc(bufferSize);
    if (buffer == NULL) {
        Serial.println("[ERROR]: Memory allocation failed");
        return false;
    }

    size_t packedSize = pairing__pairing_message__pack(&message, buffer);
    if (packedSize != bufferSize) {
        Serial.println("[ERROR]: Packing failed");
        free(buffer);
        return false;
    }
    Serial.printf("Encoded size: %zu bytes\n", packedSize);

    if (client.write(buffer, bufferSize)) {
        Serial.println("[ERROR]: Sending failed");
        free(buffer);
        return false;
    }

    Serial.println("[DEBUG]: Sent Pairing Request");

    free(buffer);
    return true;
}

bool PairingMessageManager::sendPairingRequest(WiFiClientSecure &client, const char *service_name) {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingRequest request = PAIRING__PAIRING_REQUEST__INIT;
    message.pairing_request = &request;

    message.pairing_request->service_name = (char *)service_name;
    message.pairing_request->client_name = model;

    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;
    return send(client, message);
}

bool PairingMessageManager::sendPairingOption(WiFiClientSecure &client) {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingOption option = PAIRING__PAIRING_OPTION__INIT;
    Pairing__PairingEncoding encoding = PAIRING__PAIRING_ENCODING__INIT;

    option.preferred_role = PAIRING__ROLE_TYPE__ROLE_TYPE_INPUT;

    encoding.type = PAIRING__PAIRING_ENCODING__ENCODING_TYPE__ENCODING_TYPE_HEXADECIMAL;
    encoding.symbol_length = 6;

    message.pairing_option = &option;
    message.pairing_option->preferred_role = PAIRING__ROLE_TYPE__ROLE_TYPE_INPUT;
    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;
    return send(client, message);
}

bool PairingMessageManager::sendPairingConfiguration(WiFiClientSecure &client) {
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
    return send(client, message);
}

bool PairingMessageManager::sendPairingSecret(WiFiClientSecure &client, const uint8_t *secret) {
    Pairing__PairingMessage message = PAIRING__PAIRING_MESSAGE__INIT;
    Pairing__PairingSecret secretMessage = PAIRING__PAIRING_SECRET__INIT;
    message.pairing_secret = &secretMessage;

    message.pairing_secret->secret.data = (uint8_t *)secret;
    message.pairing_secret->secret.len = 32;

    message.status = PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK;
    message.protocol_version = 2;
    return send(client, message);
}