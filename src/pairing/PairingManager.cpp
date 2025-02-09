#include "pairing/PairingManager.h"
#include "utils.h"

PairingMessageManager pairingMessageManager;

void addModulusAndExponent(WOLFSSL_X509* cert, Sha256 &sha256) {
    // Lấy Public Key từ chứng chỉ
    WOLFSSL_EVP_PKEY* pubKey = wolfSSL_X509_get_pubkey(cert);
    if (!pubKey) {
        printf("Lỗi: Không lấy được public key từ chứng chỉ.\n");
        return;
    }

    if (wolfSSL_EVP_PKEY_id(pubKey) != EVP_PKEY_RSA) {
        Serial.println("[ERROR]: Public key is not RSA");
        wolfSSL_EVP_PKEY_free(pubKey);
        return;
    }

    Serial.println("[DEBUG]: Successfully got RSA public key");

    // // get modulus and exponent


    WOLFSSL_RSA *rsaKey = pubKey->rsa;

    Serial.println("[DEBUG]: Successfully got RSA key");

    // rsaKey->n
    int modulusBitSize = mp_unsigned_bin_size((mp_int *)rsaKey->n->internal);
    if (modulusBitSize <= 0) {
        Serial.println("[ERROR]: Failed to get modulus size");
        return;
    }
    unsigned char* modulusBytes = (unsigned char*)malloc(modulusBitSize);
    if (mp_to_unsigned_bin((mp_int*)rsaKey->n->internal, modulusBytes) != MP_OKAY) {
        Serial.println("[ERROR]: Failed to convert modulus to bytes");
        free(modulusBytes);
        return;
    }
    // printPacket(modulusBytes, modulusBitSize);
    wc_Sha256Update(&sha256, modulusBytes, modulusBitSize);
    free(modulusBytes);

    int exponentBitSize = mp_unsigned_bin_size((mp_int *)rsaKey->e->internal);
    if (exponentBitSize <= 0) {
        Serial.println("[ERROR]: Failed to get exponent size");
        return;
    }
    unsigned char* exponentBytes = (unsigned char*)malloc(exponentBitSize);
    if (mp_to_unsigned_bin((mp_int*)rsaKey->e->internal, exponentBytes) != MP_OKAY) {
        Serial.println("[ERROR]: Failed to convert exponent to bytes");
        free(exponentBytes);
        return;
    }
    // printPacket(exponentBytes, exponentBitSize);
    wc_Sha256Update(&sha256, exponentBytes, exponentBitSize);
    free(exponentBytes);

    wolfSSL_EVP_PKEY_free(pubKey);
}


bool PairingManager::sendCode(const String& code) {
    if (code.length() != 6) {
        Serial.println("[ERROR]: Invalid code length!");
        return false;
    }

    Serial.printf("[DEBUG]: Sending code: %s\n", code);

    // TODO: làm tiếp 
    // NOTE: không biết là nó thêm vào kiểu gì nên đợi về nhà xem mới biết được 
    WOLFSSL_X509 *server_cert = ssl_get_peer_certificate();
    WOLFSSL_X509 *client_cert = ssl_get_certificate();

    byte hash[32];
    Sha256 sha256[1];
    if (wc_InitSha256(sha256) != 0) {
        Serial.println("[ERROR]: SHA256 initialization failed!");
        return false;
    }

    Serial.println("[DEBUG]: Adding modulus and exponent to hash");

    addModulusAndExponent(client_cert, *sha256);
    addModulusAndExponent(server_cert, *sha256);

    // TODO: thêm code vào sha256
    unsigned char codeBytes[3] = {0,0,0};
    for (int i = 0; i < 6; i+=2) {
        codeBytes[i/2] = strtol(code.substring(i, i+2).c_str(), NULL, 16);
    }

    wc_Sha256Update(sha256, codeBytes+1, 2);

    wc_Sha256Final(sha256, hash);

    Serial.println("[DEBUG]: Hash:");
    printPacket(hash, 32);
    Serial.printf("[DEBUG]: check sum: ");
    printPacket(codeBytes, 3);

    if (hash[0] != codeBytes[0] ) {
        Serial.println("[ERROR]: Checksum failed!");
        return false;
    }
    uint8_t* buffer = pairingMessageManager.createPairingSecret((uint8_t *) hash);
    ssl_send((char *) buffer, buffer[0] + 1);
    free(buffer);
    return true;
}

bool PairingManager::connected() {
    return ssl_connected();
}

void PairingManager::begin(IPAddress host, uint16_t port, char* service_name) {


    if (ssl_connect(host, port) < 0) {
        Serial.println("[ERROR]: Connection failed!");
        return;
    }

    Serial.printf("[DEBUG]: %s Pairing connected\n", host.toString());
    uint8_t* buffer = pairingMessageManager.createPairingRequest(service_name);
    ssl_send((char *) buffer, buffer[0] + 1);
    free(buffer);
}

void PairingManager::loop() {
    uint8_t buffer[256];
    if (ssl_available() <= 0) {
        return;
    }
    int len = ssl_read((char *) buffer, sizeof(buffer));
    if (len <= 0) {
        Serial.println("[ERROR]: Read failed");
        return;
    }
    Serial.println();
    chunks.insert(chunks.end(), buffer, buffer + len);

    if (chunks.size() > 0 && chunks[0] == chunks.size() - 1) {
        printPacket(chunks.data(), chunks.size());        
        Pairing__PairingMessage *response = pairing__pairing_message__unpack(NULL, chunks.size() - 1, chunks.data() + 1);
        if (response->status != PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK) {
            ssl_stop();
            Serial.println(response->status);
        } else {
            handleResponse(response);
        }
        chunks.clear();

        pairing__pairing_message__free_unpacked(response, NULL);
    }
}

void PairingManager::handleResponse(Pairing__PairingMessage *message) {
    if (message->pairing_request_ack) {
        Serial.printf("[DEBUG]: Pairing request ack\n");
        uint8_t* buffer = pairingMessageManager.createPairingOption();
        ssl_send((char *) buffer, buffer[0] + 1);
        free(buffer);
    } 
    else if (message->pairing_option) {
        uint8_t* buffer = pairingMessageManager.createPairingConfiguration();
        ssl_send((char *) buffer, buffer[0] + 1);
        free(buffer);
    } 
    else if (message->pairing_configuration_ack) {
        Serial.printf("[DEBUG]: Pairing configuration ack\n");
        isSecure = true;
    } 
    else if (message->pairing_secret_ack) {
        isSecure = false;
        Serial.printf("[DEBUG]: Paired!\n");
        ssl_stop();
    } 
    else {
        Serial.printf("[ERROR]: What Else?\n");
    }
}
