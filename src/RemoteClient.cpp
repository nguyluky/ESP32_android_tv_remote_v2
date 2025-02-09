#include "RemoteClient.h"
WiFiClient client;

static WOLFSSL_CTX* ctx = NULL;
static WOLFSSL* ssl = NULL;
static char wc_error_message[80 + 1];
static char errBuf[80];
static char *cert_buffer = NULL;
static char *key_buffer = NULL;

static int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx) {
    int sent = 0;
    (void)ssl;
    (void)ctx;

    sent = client.write((byte*)msg, sz);
    return sent;
}
static int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx) {
    int ret = 0;
    (void)ssl;
    (void)ctx;

    while (client.available() > 0 && ret < sz) {
        reply[ret++] = client.read();
    }
    return ret;
}

int setup_wolfssl(void) {
    int ret = 0;
    WOLFSSL_METHOD* method = NULL;

    ret = wolfSSL_Init();
    if (ret != WOLFSSL_SUCCESS) {
        Serial.println("[ERROR]: wolfSSL_Init failed");
    }
    Serial.println("[DEBUG]: Successfully called wolfSSL_Init");

    method = wolfTLSv1_3_client_method();
    if (method == NULL) {
        Serial.println("[ERROR]: unable to get wolfssl client method");
        return -1;
    }
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        Serial.println("[ERROR]: unable to get ctx");
        return -1;
    }
    return ret;
}

int setup_certificates(void) {
    int ret = 0;

    Serial.println("[DEBUG]: Initializing certificates...");
    
    wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, NULL);

    // NOTE: not verifying the server certificate
    // ret = wolfSSL_CTX_load_verify_buffer(ctx, CTX_CA_CERT, CTX_CA_CERT_SIZE, CTX_CA_CERT_TYPE);
    // if (ret == WOLFSSL_SUCCESS) {
    //     Serial.println("[DEBUG]: Success: load_verify CTX_CA_CERT");
    // }
    // else {
    //     Serial.println("[ERROR]: wolfSSL_CTX_load_verify_buffer failed: ");
    //     wc_ErrorString(ret, wc_error_message);
    //     Serial.println(wc_error_message);
    //     return -1;
    // }

    ret = wolfSSL_CTX_use_certificate_buffer(ctx, CTX_CLIENT_CERT, CTX_CLIENT_CERT_SIZE, CTX_CLIENT_CERT_TYPE);
    if (ret == WOLFSSL_SUCCESS) {
        Serial.println("[DEBUG]: Success: use certificate: ");
        Serial.println(xstr(CTX_SERVER_CERT));
    }
    else {
        Serial.println("[ERROR]: wolfSSL_CTX_use_certificate_buffer failed: ");
        wc_ErrorString(ret, wc_error_message);
        Serial.println(wc_error_message);
        return -1;
    }

    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, CTX_CLIENT_KEY, CTX_CLIENT_KEY_SIZE, CTX_CLIENT_KEY_TYPE);
    if (ret == WOLFSSL_SUCCESS) {
        Serial.println("[DEBUG]: Success: use private key buffer: ");
        Serial.println(xstr(CTX_SERVER_KEY));
    }
    else {
        Serial.println("[ERROR]: wolfSSL_CTX_use_PrivateKey_buffer failed: ");
        wc_ErrorString(ret, wc_error_message);
        Serial.println(wc_error_message);
        return -1;
    }

    return ret;
}

int error_check(int this_ret, const __FlashStringHelper* message) {
    if (this_ret == WOLFSSL_SUCCESS) {
        Serial.print("[DEBUG]: Success: ");
        Serial.println(message);
        return 1;
    }
    else {
        Serial.print("[ERROR]: return = ");
        Serial.print(this_ret);
        Serial.print(": ");
        Serial.println(message);
        Serial.println(wc_GetErrorString(this_ret));
        return -1;
    }
}

int error_check_ssl(WOLFSSL* ssl, int this_ret, const __FlashStringHelper* message) {
    int err = 0;

    if (ssl == NULL) {
        Serial.println("[ERROR]: ssl is Null; Unable to allocate SSL object?");
        Serial.println("[ERROR]: See wolfssl/wolfcrypt/error-crypt.h for codes.");
        Serial.print("[ERROR]: ");
        Serial.println(message);
    }
    else {
        err = wolfSSL_get_error(ssl, this_ret);
        if (err == WOLFSSL_SUCCESS) {
            Serial.print("[DEBUG]: Success: ");
            Serial.println(message);
        }
        else {
            Serial.print("[ERROR]: ");
            Serial.println(message);
            Serial.println(wc_GetErrorString(err));
        }
    }
    return err;
}



// ==========================================
int ssl_connect(IPAddress ip, uint16_t port) {

    if (client.connected()) {
        Serial.println("[ERROR]: Already connected to the server");
        return -1;
    }

    setup_wolfssl();
    setup_certificates();
    // TODO: Implement setup_certificates
    // wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, NULL);
    
    wolfSSL_SetIOSend(ctx, EthernetSend);
    wolfSSL_SetIORecv(ctx, EthernetReceive);

    int ret = client.connect(ip, port);
    if (ret == 0) {
        Serial.println("[ERROR]: Connection failed");
        return -1;
    }
    
    Serial.println("[DEBUG]: Successfully connected to the server");

    ret = wolfSSL_Init();
    if (error_check(ret, F("calling wolfSSL_Init")) < 0) {
        return -1;
    }

    ssl = wolfSSL_new(ctx);
    error_check_ssl(ssl, 0, F("Create WOLFSSL object from ctx"));

    int err = 0;
    do {
        err = 0;
        ret = wolfSSL_connect(ssl);
        if ((ret != WOLFSSL_SUCCESS) && (ret != WC_PENDING_E)) {
            err = error_check_ssl(ssl, ret, F("Create WOLFSSL object from ctx"));
        }
    } while (err == WC_PENDING_E);    

    Serial.println("[DEBUG]: Successfully connected to the server");

    return 1;
}

uint8_t ssl_send(const char* msg, int msgSz) {
    int ret = wolfSSL_write(ssl, msg, msgSz);
    return ret;
}

int ssl_available() {
    return client.available();
}

int ssl_read(char* reply, int sz) {
    return wolfSSL_read(ssl, reply, sz);
}

void ssl_stop() {
    if (ssl != NULL) {
        wolfSSL_free(ssl);
    }
    if (ctx != NULL) {
        wolfSSL_CTX_free(ctx);
    }
    client.stop();
}

uint8_t ssl_connected() {
    return client.connected();
}

WOLFSSL_X509 *ssl_get_peer_certificate() {
    return wolfSSL_get_peer_certificate(ssl);
}

WOLFSSL_X509* ssl_get_certificate() {
    return wolfSSL_get_certificate(ssl);
}