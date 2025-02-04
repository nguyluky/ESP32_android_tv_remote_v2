
#include <WiFiClientSecure.h>
#include "remote/remotemessage.pb-c.h"

#ifndef REMOTE_MESSAGE_MANAGER_H
#define REMOTE_MESSAGE_MANAGER_H
class RemoteMessageManager {
public:
    RemoteMessageManager() ;
    bool sendMessageConfig(WiFiClientSecure &client, char *model, char *vendor) ;
    bool sendRemoteSetActive(WiFiClientSecure &client, bool active) ;
    bool sendRemotePingResponse(WiFiClientSecure &client, int32_t val1) ;
    bool sendRemoteKeyInject(WiFiClientSecure &client, Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction) ;
    // TODO: chưa biết cái này làm sao
    // bool sendRemoteAdjustVolumeLevel(WiFiClientSecure &client, ) {
    //     Remote__RemoteMessage message = REMOTE__REMOTE_MESSAGE__INIT;
    //     Remote__RemoteAdjustVolumeLevel volumeLevel = REMOTE__REMOTE_ADJUST_VOLUME_LEVEL__INIT;

    //     volumeLevel.base


    //     message.message_adjust_volume_level = &volumeLevel;

    //     return send(client, message);
    // }
    bool sendRemoteResetPreferredAudioDevice(WiFiClientSecure &client) ;
    bool sendRemoteImeKeyInject(WiFiClientSecure &client, char *appPackage, Remote__RemoteTextFieldStatus status) ;
    bool sendRemoteRemoteAppLinkLaunchRequest(WiFiClientSecure &client, char *appLink);
};


extern RemoteMessageManager remoteMessageManager;
#endif