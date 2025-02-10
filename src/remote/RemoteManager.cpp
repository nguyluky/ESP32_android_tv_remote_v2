#include "remote/RemoteManager.h"

void RemoteManager::start(IPAddress ip, uint16_t port)
{

    if (ssl_connected())
    {
        Serial.println("[ERROR]: Already connected to the server");
        return;
    }

    int err = ssl_connect(ip, port);
    if (err < 0)
    {
        Serial.println("[ERROR]: Connection failed!");
        return;
    }

    Serial.printf("[DEBUG]: %s:%d Pairing connected\n", ip.toString(), port);
}

void RemoteManager::loop()
{

    if (!ssl_connected())
    {
        return;
    }

    if (ssl_available() <= 0)
    {
        return;
    }

    uint8_t buffer[256];
    int len = ssl_read((char *)buffer, sizeof(buffer));
    if (len <= 0)
    {
        Serial.println("[ERROR]: Read failed");
        stop();
        error_auth = true;
        return;
    }
    chunks.insert(chunks.end(), buffer, buffer + len);

    if (chunks.size() > 0 && chunks[0] == chunks.size() - 1)
    {
        printPacket(chunks.data(), chunks.size());
        Remote__RemoteMessage *message = remote__remote_message__unpack(NULL, chunks.size() - 1, chunks.data() + 1);

        if (message->remote_configure)
        {
            uint8_t *buffer = remoteMessageManager.createMessageConfig("Mi TV", "Xiaomi");
            ssl_send((char *)buffer, buffer[0] + 1);
            free(buffer);
        }

        else if (message->remote_set_active)
        {
            uint8_t *buffer = remoteMessageManager.createRemoteSetActive(true);
            ssl_send((char *)buffer, buffer[0] + 1);
            free(buffer);
        }

        else if (message->remote_ping_request)
        {
            uint8_t *buffer = remoteMessageManager.createRemotePingResponse(message->remote_ping_request->val1);
            ssl_send((char *)buffer, buffer[0] + 1);
            free(buffer);
        }
        else if (message->remote_ime_batch_edit)
        {
            Serial.print("[INFO]: IME Batch Edit");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_ime_show_request)
        {
            Serial.print("[INFO]: IME Show Request");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_voice_begin)
        {
            Serial.print("[INFO]: Voice Begin");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_voice_payload)
        {
            Serial.print("[INFO]: Voice Payload");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_voice_end)
        {
            Serial.print("[INFO]: Voice End");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_start)
        {
            Serial.print("[INFO]: Start");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_set_volume_level)
        {
            Serial.print("[INFO]: Set Volume Level");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_set_preferred_audio_device)
        {
            Serial.print("[INFO]: Set Preferred Audio Device");
            printPacket(chunks.data(), chunks.size());
        }
        else if (message->remote_error)
        {
            Serial.print("[ERROR]: ");
            printPacket(chunks.data(), chunks.size());
        }
        else
        {
            Serial.print("[INFO]: Unknown message");
            printPacket(chunks.data(), chunks.size());
        }

        chunks.clear();
        remote__remote_message__free_unpacked(message, NULL);
    }
}

bool RemoteManager::sendPower()
{
    uint8_t *buffer = remoteMessageManager.createRemoteKeyInject(REMOTE__REMOTE_KEY_CODE__KEYCODE_POWER, REMOTE__REMOTE_DIRECTION__SHORT);
    ssl_send((char *)buffer, buffer[0] + 1);
    free(buffer);
    return true;
}

bool RemoteManager::sendKey(Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction)
{
    // return remoteMessageManager.sendRemoteKeyInject(client, keyCode, direction);
    uint8_t *buffer = remoteMessageManager.createRemoteKeyInject(keyCode, direction);
    ssl_send((char *)buffer, buffer[0] + 1);
    free(buffer);
    return true;
}

bool RemoteManager::sendAppLink(char *appLink)
{
    // return remoteMessageManager.sendRemoteRemoteAppLinkLaunchRequest(client, appLink);

    uint8_t *buffer = remoteMessageManager.createRemoteRemoteAppLinkLaunchRequest(appLink);
    ssl_send((char *)buffer, buffer[0] + 1);
    free(buffer);
    return true;
}

bool RemoteManager::stop()
{
    ssl_stop();
    return true;
}

bool RemoteManager::connected()
{
    return ssl_connected();
}