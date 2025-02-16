#ifndef UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include "pairing/PairingManager.h"
#include "remote/RemoteManager.h"
#include "remote/RemoteKeycode.h"

const char *ssid = "SieuCoi";
const char *password = "0902838500";

RemoteManager remoteManager;
PairingManager pairingManager;

String macAddress = "40:AA:56:62:5E:06";

const uint8_t pin_switch[] = {
    GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26,
    GPIO_NUM_27, GPIO_NUM_14,

    GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21,
    GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5};

const uint8_t keys_pin[][2] = {
    {7, 8}, {7, 11}, {7, 10}, {6, 7}, {6, 8},
    {6, 11}, {6, 10}, {8, 9}, {9, 11}, {9, 10}, 
    {0, 8}, {0, 11}, {0, 10}, {1, 8}, {1, 11}, 
    {1, 10}, {2, 8}, {2, 11}, {2, 10}, {10, 11}, 
    {3, 8}, {3, 11}, {3, 10}, {8, 10}, {4, 8}, 
    {4, 11}, {4, 10}
};

bool isPressed[27] = {false};

IPAddress tv_ip;

void sendKey(int key)
{
    switch (key)
    {
    case 0:
        remoteManager.sendKey(KEYCODE_VOLUME_UP, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 1:
        remoteManager.sendKey(KEYCODE_POWER, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 2:
        remoteManager.sendKey(KEYCODE_VOLUME_DOWN, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 3:
        // youtube app
        // remoteManager.sendAppLink();
        break;
    case 4:
        // netflix app
        // remoteManager.sendAppLink();
        break;
    case 5:
        // 
        break;
    case 6:
        break;
    case 7:
        remoteManager.sendKey(KEYCODE_1, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 8:
        remoteManager.sendKey(KEYCODE_2, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 9:
        remoteManager.sendKey(KEYCODE_3, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 10:
        remoteManager.sendKey(KEYCODE_4, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 11:
        remoteManager.sendKey(KEYCODE_5, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 12:
        remoteManager.sendKey(KEYCODE_6, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 13:
        remoteManager.sendKey(KEYCODE_7, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 14:
        remoteManager.sendKey(KEYCODE_8, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 15:
        remoteManager.sendKey(KEYCODE_9, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 16:
        remoteManager.sendKey(KEYCODE_BACK, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 17:
        remoteManager.sendKey(KEYCODE_0, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 18:
        remoteManager.sendKey(KEYCODE_HOME, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    
    // ====================

    case 21:
        remoteManager.sendKey(KEYCODE_DPAD_CENTER, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 19:
        remoteManager.sendKey(KEYCODE_DPAD_UP, REMOTE__REMOTE_DIRECTION__SHORT);
        break; 
    case 20:
        remoteManager.sendKey(KEYCODE_DPAD_LEFT, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 22:
        remoteManager.sendKey(KEYCODE_DPAD_RIGHT, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    case 23:
        remoteManager.sendKey(KEYCODE_DPAD_DOWN, REMOTE__REMOTE_DIRECTION__SHORT);
        break;
    default:
        break;
    }
}


void setup()
{
    Serial.begin(115200);
    Serial.println("Hello, world!");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    for (int j = 0; j < 12; j++) {
        pinMode(pin_switch[j], INPUT_PULLUP);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    tv_ip = findTVIp(macAddress);

    digitalWrite(LED_BUILTIN, LOW);

    remoteManager.start(tv_ip, 6466);

    Serial.println("[INFO]: start loop");
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    remoteManager.loop();

    if (remoteManager.error_auth)
    {
        pairingManager.begin(tv_ip, 6467, "service_name");
        while (pairingManager.connected())
        {
            pairingManager.loop();
            if (pairingManager.isSecure)
            {
                Serial.println("Enter code: ");
                if (Serial.available() > 0)
                {
                    String code = Serial.readString();
                    Serial.println(code);
                    if (code.length() == 6)
                    {
                        pairingManager.sendCode(code);
                    }
                }
            }
            delay(1000);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        remoteManager.error_auth = false;

        remoteManager.start(tv_ip, 6466);
        digitalWrite(LED_BUILTIN, HIGH);
    }

    for (int i = 0; i < 27; i++) {

        pinMode(pin_switch[keys_pin[i][0]], OUTPUT);
        digitalWrite(pin_switch[keys_pin[i][0]], LOW);

        if (digitalRead(pin_switch[keys_pin[i][1]]) == LOW) {
            if (!isPressed[i]) {
                isPressed[i] = true;
                Serial.printf("Key: %d\n", i);
                sendKey(i);
            }
        }    
        else {
            isPressed[i] = false;
        }

        pinMode(pin_switch[keys_pin[i][0]], INPUT_PULLUP);

    }

    delay(100);
}
#endif