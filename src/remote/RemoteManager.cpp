#include <Arduino.h>
#include <vector>
#include <WiFiClientSecure.h>
#include "remote/remotemessage.pb-c.h"
#include "remote/RemoteMessageManager.h"
#include "certificate/CertificateGenerator.h"
#include "utils.h"


class RemoteManager {
public:
    RemoteManager(char *host, int port);
    void start() {
        client.setCertificate(client_cert_pem);   // Cài đặt chứng chỉ
        client.setPrivateKey(client_key_pem);

        if (!client.connect(host, port)) {
            Serial.println("{ERROR]: Connection failed!");
            return;
        }

        Serial.printf("[DEBUG]: %s Pairing connected\n", host);
    }

    void loop() {

        // this function will be called in loop

        if (!client.connected()) {
            return;
        }

        if (!client.available()) {
            return;
        }

        uint8_t buffer[256];
        size_t len = client.read(buffer, sizeof(buffer));
        chunks.insert(chunks.end(), buffer, buffer + len);

        if (chunks.size() > 0 && chunks[0] == chunks.size() - 1) {
            Remote__RemoteMessage *message = remote__remote_message__unpack(NULL, chunks.size() - 1, chunks.data() + 1);

            if (message->remote_configure) {
                uint8_t *buffer = remoteMessageManager.createMessageConfig("Mi TV", "Xiaomi");
                client.write(buffer, buffer[0] + 1);
            }
            else if (message->remote_set_active) {
                uint8_t *buffer = remoteMessageManager.createRemoteSetActive(true);
                client.write(buffer, buffer[0] + 1);
            }
            else if (message->remote_ping_request) {
                // remoteMessageManager.sendRemotePingResponse(client, message->remote_ping_request->val1);
                uint8_t *buffer = remoteMessageManager.createRemotePingResponse(message->remote_ping_request->val1);
                client.write(buffer, buffer[0] + 1);
            }
            else if (message->remote_ime_batch_edit) {
                Serial.print("[INFO]: IME Batch Edit");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_ime_show_request) {
                Serial.print("[INFO]: IME Show Request");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_voice_begin) {
                Serial.print("[INFO]: Voice Begin");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_voice_payload) {
                Serial.print("[INFO]: Voice Payload");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_voice_end) {
                Serial.print("[INFO]: Voice End");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_start) {
                Serial.print("[INFO]: Start");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_set_volume_level) {
                Serial.print("[INFO]: Set Volume Level");
                printPacket(chunks.data(), chunks.size());
            }
            else if (message->remote_set_preferred_audio_device) {
                Serial.print("[INFO]: Set Preferred Audio Device");
                printPacket(chunks.data(), chunks.size()); 
            }
            else if (message->remote_error) {
                Serial.print("[ERROR]: ");
                printPacket(chunks.data(), chunks.size());
            }
            else {
                Serial.print("[INFO]: Unknown message");
                printPacket(chunks.data(), chunks.size());
            }

            chunks.clear();
            remote__remote_message__free_unpacked(message, NULL);
        }
    }

    bool sendPower() {
        uint8_t *buffer = remoteMessageManager.createRemoteKeyInject(REMOTE__REMOTE_KEY_CODE__KEYCODE_POWER, REMOTE__REMOTE_DIRECTION__SHORT);
        client.write(buffer, buffer[0] + 1);
        return true;
    }
    
    bool sendKey(Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction) {
        // return remoteMessageManager.sendRemoteKeyInject(client, keyCode, direction);
        uint8_t *buffer = remoteMessageManager.createRemoteKeyInject(keyCode, direction);
        client.write(buffer, buffer[0] + 1);
        return true;
    }

    bool sendAppLink(char *appLink) {
        // return remoteMessageManager.sendRemoteRemoteAppLinkLaunchRequest(client, appLink);

        uint8_t *buffer = remoteMessageManager.createRemoteRemoteAppLinkLaunchRequest(appLink);
        client.write(buffer, buffer[0] + 1);
        return true;
    } 

    bool stop() {
        // TODO: Implement stop function
        return true;
    }

private:
    char *host;
    uint8_t port;
    std::vector<uint8_t> chunks;
    WiFiClientSecure client;
};