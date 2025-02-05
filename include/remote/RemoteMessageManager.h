
#include <WiFiClientSecure.h>
#include "remote/remotemessage.pb-c.h"

#ifndef REMOTE_MESSAGE_MANAGER_H
#define REMOTE_MESSAGE_MANAGER_H
class RemoteMessageManager {
public:
    uint8_t* createMessageConfig(char *model, char *vendor) ;
    uint8_t* createRemoteSetActive(bool active) ;
    uint8_t* createRemotePingResponse(int32_t val1) ;
    uint8_t *createRemoteKeyInject(Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction) ;
    uint8_t *createRemoteResetPreferredAudioDevice() ;
    uint8_t *createRemoteImeKeyInject(char *appPackage, Remote__RemoteTextFieldStatus status) ;
    uint8_t *createRemoteRemoteAppLinkLaunchRequest(char *appLink);

    uint8_t* encodeMessage(Remote__RemoteMessage &message) ;
};


extern RemoteMessageManager remoteMessageManager;
#endif