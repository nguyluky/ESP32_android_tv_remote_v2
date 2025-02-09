#ifndef _REMOTE_MANAGER_H_
#define _REMOTE_MANAGER_H_
#include <Arduino.h>
#include <vector>
#include "remote/remotemessage.pb-c.h"
#include "remote/RemoteMessageManager.h"
#include "utils.h"
#include "RemoteClient.h"

class RemoteManager {
public:
    void start(IPAddress ip, uint16_t port);
    void loop() ;
    bool sendPower() ;
    bool sendKey(Remote__RemoteKeyCode keyCode, Remote__RemoteDirection direction) ;
    bool sendAppLink(char *appLink) ;
    bool stop() ;
    bool connected() ;
    bool error_auth = false;
private:
    std::vector<uint8_t> chunks;
};
#endif