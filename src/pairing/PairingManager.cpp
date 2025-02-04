#include "pairing/PairingManager.h"

PairingManager::PairingManager(char* host, uint16_t port, char* service_name) {
    this->host = host;
    this->port = port;
    this->service_name = service_name;

}
    // : host(host), port(port), service_name(service_name), client() {}

bool PairingManager::sendCode(const String& code) {
    if (code.length() != 6) {
        Serial.println("[ERROR]: Invalid code length!");
        return false;
    }

    Serial.printf("[DEBUG]: Sending code: %s\n", code);

    const mbedtls_x509_crt *server_cert = client.getPeerCertificate();
    mbedtls_x509_crt *client_cert;

    mbedtls_x509_crt_init(client_cert);
    mbedtls_x509_crt_parse(client_cert, (const unsigned char *)client_cert_pem, strlen(client_cert_pem) + 1);

    const mbedtls_rsa_context* client_rsa = mbedtls_pk_rsa(client_cert->pk);
    const mbedtls_rsa_context* server_rsa = mbedtls_pk_rsa(server_cert->pk);

    std::vector<uint8_t> last4 = hexStringToBytes(code.substring(2, 6));

    SHA256 sha256;
    uint8_t hash[32];

    sha256.update(client_rsa->N.p, client_rsa->N.n);
    sha256.update(client_rsa->E.p, client_rsa->E.n);
    sha256.update(server_rsa->N.p, server_rsa->N.n);
    sha256.update(server_rsa->E.p, server_rsa->E.n);
    sha256.update(last4.data(), last4.size());

    sha256.finalize(hash, sizeof(hash));
    
    // TODO: check if the hash is correct
    // std::vector<uint8_t> hashVec = hexStringToBytes();

    pairingMessageManager.sendPairingSecret(client, hash);
    return true;
}

void PairingManager::start() {
    client.setCertificate(client_cert_pem);   // Cài đặt chứng chỉ
    client.setPrivateKey(client_key_pem); 

    if (!client.connect(host, port)) {
        Serial.println("{ERROR]: Connection failed!");
        return;
    }

    Serial.printf("[DEBUG]: %s Pairing connected\n", host);
    pairingMessageManager.sendPairingRequest(client, service_name);

    while (client.connected()) {
        while (client.available()) {
            uint8_t buffer[256];
            size_t len = client.read(buffer, sizeof(buffer));
            chunks.insert(chunks.end(), buffer, buffer + len);

            if (chunks.size() > 0 && chunks[0] == chunks.size() - 1) {
                Pairing__PairingMessage *response = pairing__pairing_message__unpack(NULL, chunks.size() - 1, chunks.data() + 1);
                if (response->status != PAIRING__PAIRING_MESSAGE__STATUS__STATUS_OK) {
                    client.stop();
                    Serial.println(response->status);
                } else {
                    handleResponse(response);
                }
                chunks.clear();

                pairing__pairing_message__free_unpacked(response, NULL);
            }
        }
    }

    Serial.printf("[DEBUG]: %s Pairing Connection closed\n", host);
}

std::vector<uint8_t> PairingManager::hexStringToBytes(const String &hexString) {
    std::vector<uint8_t> bytes;

    // Chắc chắn rằng chuỗi hex có độ dài chẵn
    if (hexString.length() % 2 != 0) {
        Serial.println("Chuỗi hex phải có độ dài chẵn.");
        return bytes;
    }

    // Lặp qua từng cặp ký tự trong chuỗi hex và chuyển đổi thành byte
    for (int i = 0; i < hexString.length(); i += 2) {
        String byteString = hexString.substring(i, i + 2);  // Lấy cặp ký tự
        uint8_t byte = strtol(byteString.c_str(), NULL, 16);  // Chuyển đổi hex thành byte
        bytes.push_back(byte);  // Thêm vào mảng bytes
    }

    return bytes;  // Trả về mảng byte
}   
void PairingManager::handleResponse(Pairing__PairingMessage *message) {
    if (message->pairing_configuration_ack) {
        pairingMessageManager.sendPairingConfiguration(client);
    } 
    else if (message->pairing_option) {
        pairingMessageManager.sendPairingOption(client);
    } 
    else if (message->pairing_configuration_ack) {
        // Emit 'secret' event
        // await input from user
    } 
    else if (message->pairing_secret_ack) {
        Serial.printf("[DEBUG]: %s Paired!\n", host);
        client.stop();
    } 
    else {
        Serial.printf("[ERROR]: %s What Else?\n", host);
    }
}
