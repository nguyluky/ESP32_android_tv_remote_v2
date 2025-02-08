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

    // get modulus and exponent

    RsaKey rsaKey;
    word32 idx = 0;
    byte der[2048];
    byte* pdep = der;
    int derSz = wolfSSL_i2d_PublicKey(pubKey, (unsigned char **)&pdep);
    if (derSz <= 0) {
        printf("Failed to convert public key to DER format\n");
        return;
    }

    
    // Giải mã DER để lấy thông tin RSA
    wc_InitRsaKey(&rsaKey, NULL);
    if (wc_RsaPublicKeyDecode(der, &idx, &rsaKey, derSz) != 0) {
        printf("Failed to decode RSA public key\n");
        return;
    }


    // en

    
    wc_Sha256Update(&sha256, (byte *)rsaKey.n.dp, rsaKey.n.used * sizeof(fp_digit));
    // add 3 bytes ex
    byte ex[1] = {0};
    wc_Sha256Update(&sha256, ex, 1);
    wc_Sha256Update(&sha256, (byte *)&rsaKey.e.dp, rsaKey.e.used * sizeof(fp_digit));

    // Giải phóng bộ nhớ
    wc_FreeRsaKey(&rsaKey);
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

    addModulusAndExponent(client_cert, *sha256);
    addModulusAndExponent(server_cert, *sha256);

    // TODO: thêm code vào sha256

    wc_Sha256Final(sha256, hash);

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
    int len = ssl_read((char *) buffer, sizeof(buffer));
    if (len <= 0) {
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
        // Emit 'secret' event
        // await input from user

        // TODO: thêm call back
        Serial.printf("[DEBUG]: Pairing configuration ack--------------------------------------\n");
    } 
    else if (message->pairing_secret_ack) {
        Serial.printf("[DEBUG]: Paired!\n");
        ssl_stop();
    } 
    else {
        Serial.printf("[ERROR]: What Else?\n");
    }
}
