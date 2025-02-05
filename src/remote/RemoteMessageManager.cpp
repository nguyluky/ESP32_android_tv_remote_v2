// #include <WiFiClientSecure.h>
// #include "remote/remotemessage.pb-c.h"
#include "remote/RemoteMessageManager.h"

RemoteMessageManager remoteMessageManager;

uint8_t* RemoteMessageManager::encodeMessage(Remote__RemoteMessage &message)
{
    size_t bufferSize = remote__remote_message__get_packed_size(&message);
    if (bufferSize == 0)
    {
        Serial.println("[ERROR]: Buffer size is 0");
        return NULL;
    }

    uint8_t *buffer = (uint8_t *)malloc(bufferSize + 1);
    if (buffer == NULL)
    {
        Serial.println("[ERROR]: Memory allocation failed");
        return NULL;
    }

    buffer[0] = bufferSize;

    size_t packedSize = remote__remote_message__pack(&message, buffer + 1);
    if (packedSize != bufferSize)
    {
        Serial.println("[ERROR]: Packing failed");
        free(buffer);
        return NULL;
    }
    Serial.printf("Encoded size: %zu bytes\n", packedSize);

    return buffer;
}

uint8_t* RemoteMessageManager::createMessageConfig(char *model, char *vendor)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteConfigure config = REMOTE__REMOTE_CONFIGURE__INIT;
    Remote__RemoteDeviceInfo device = REMOTE__REMOTE_DEVICE_INFO__INIT;

    config.code1 = 622;

    device.model = model;
    device.vendor = vendor;
    device.unknown1 = 1;
    device.unknown2 = "1";
    device.package_name = "androidtv-remote";
    device.app_version = "1.0.0";

    config.device_info = &device;
    message.remote_configure = &config;


    return encodeMessage(message);
}

uint8_t* RemoteMessageManager::createRemoteSetActive(bool active)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteSetActive _active = REMOTE__REMOTE_SET_ACTIVE__INIT;

    _active.active = active;
    message.remote_set_active = &_active;

    return encodeMessage(message);
}

uint8_t* RemoteMessageManager::createRemotePingResponse(int32_t val1)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemotePingResponse response = REMOTE__REMOTE_PING_RESPONSE__INIT;

    response.val1 = val1;

    message.remote_ping_response = &response;

    // return send(client, message);
    return encodeMessage(message);
}

uint8_t* RemoteMessageManager::createRemoteKeyInject(Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteKeyInject keyInject = REMOTE__REMOTE_KEY_INJECT__INIT;

    keyInject.key_code = keyCode;
    keyInject.direction = direction;

    message.remote_key_inject = &keyInject;

    // return send(client, message);
    return encodeMessage(message);
}

// TODO: chưa biết cái này làm sao
// bool sendRemoteAdjustVolumeLevel(WiFiClientSecure &client, ) {
//     Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
//     Remote__RemoteAdjustVolumeLevel volumeLevel = REMOTE__REMOTE_ADJUST_VOLUME_LEVEL__INIT;

//     volumeLevel.base

//     message.message_adjust_volume_level = &volumeLevel;

//     return send(client, message);
// }

uint8_t* RemoteMessageManager::createRemoteResetPreferredAudioDevice()
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteResetPreferredAudioDevice reset = REMOTE__REMOTE_RESET_PREFERRED_AUDIO_DEVICE__INIT;

    message.remote_reset_preferred_audio_device = &reset;

    // return send(client, message);
    return encodeMessage(message);
}

uint8_t* RemoteMessageManager::createRemoteImeKeyInject(char *appPackage, Remote__RemoteTextFieldStatus status)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteImeKeyInject keyInject = REMOTE__REMOTE_IME_KEY_INJECT__INIT;
    Remote__RemoteAppInfo appInfo = REMOTE__REMOTE_APP_INFO__INIT;

    keyInject.text_field_status = &status;
    appInfo.app_package = appPackage;

    keyInject.app_info = &appInfo;
    message.remote_ime_key_inject = &keyInject;

    // return send(client, message);
    return encodeMessage(message);
}

uint8_t* RemoteMessageManager::createRemoteRemoteAppLinkLaunchRequest(char *appLink)
{
    Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    Remote__RemoteAppLinkLaunchRequest request = REMOTE__REMOTE_APP_LINK_LAUNCH_REQUEST__INIT;

    request.app_link = appLink;
    message.remote_app_link_launch_request = &request;

    return encodeMessage(message);
}

// };