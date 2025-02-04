#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Crypto.h>
#include <SHA256.h>
#include <vector>
#include "pairing/PairingMessageManager.h"
#include "certificate/CertificateGenerator.h"

class PairingManager {
public:
    PairingManager(const char* host, uint16_t port, const char* service_name)
        : host(host), port(port), service_name(service_name), client() {}

    bool sendCode(const String& code) {
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

        Pairing__PairingMessage message = Pairing__PairingMessage_init_default;
        pairingMessageManager.createPairingSecret(message, hash);
        sendNanopbMessage(client, message);
        return true;
    }

    void start() {
        client.setCertificate(client_cert_pem);   // Cài đặt chứng chỉ
        client.setPrivateKey(client_key_pem); 

        if (!client.connect(host, port)) {
            Serial.println("{ERROR]: Connection failed!");
            return;
        }

        Serial.printf("[DEBUG]: %s Pairing connected\n", host);
        Pairing__PairingMessage message = Pairing__PairingMessage_init_default;
        pairingMessageManager.createPairingRequest(message, service_name);
        sendNanopbMessage(client, message);

        while (client.connected()) {
            while (client.available()) {
                uint8_t buffer[256];
                size_t len = client.read(buffer, sizeof(buffer));
                chunks.insert(chunks.end(), buffer, buffer + len);

                if (chunks.size() > 0 && chunks[0] == chunks.size() - 1) {
                    Pairing__PairingMessage response = Pairing__PairingMessage_init_default;
                    pairingMessageManager.decodeMessage(chunks.data(), chunks.size(), response);

                    if (response.status != Pairing__PairingMessage_Status_STATUS_OK) {
                        client.stop();
                        Serial.println(response.status);
                    } else {
                        handleResponse(response);
                    }
                    chunks.clear();
                }
            }
        }

        Serial.printf("[DEBUG]: %s Pairing Connection closed\n", host);
    }

private:
// Hàm chuyển chuỗi hex thành mảng byte
    std::vector<uint8_t> hexStringToBytes(const String &hexString) {
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
    void handleResponse(Pairing__PairingMessage& message) {
        if (message.has_pairing_request_ack) {
            Pairing__PairingMessage ackMessage = Pairing__PairingMessage_init_default;
            pairingMessageManager.createPairingOption(ackMessage);
            sendNanopbMessage(client, ackMessage);
        } else if (message.has_pairing_option) {
            Pairing__PairingMessage configMessage = Pairing__PairingMessage_init_default;
            pairingMessageManager.createPairingConfiguration(configMessage);
            sendNanopbMessage(client, configMessage);
        } else if (message.has_pairing_configuration_ack) {
            // Emit 'secret' event
            // await input from user
        } else if (message.has_pairing_secret_ack) {
            Serial.printf("[DEBUG]: %s Paired!\n", host);
            client.stop();
        } else {
            Serial.printf("[ERROR]: %s What Else?\n", host);
        }
    }

    const char* host;
    uint16_t port;
    const char* service_name;
    WiFiClientSecure client;
    std::vector<uint8_t> chunks;
};