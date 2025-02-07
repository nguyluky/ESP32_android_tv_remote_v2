#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H
#define WOLFSSL_CLIENT_EXAMPLE

#include <Arduino.h>
#include <wolfssl.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <WiFi.h>
#include <WiFiClient.h>

#define xstr(x) str(x)
#define str(x) #x


int ssl_connect(IPAddress ip, uint16_t port) ;
uint8_t ssl_send(const char* msg, int msgSz) ;
int ssl_available() ;
int ssl_read(char* reply, int sz) ;


#endif