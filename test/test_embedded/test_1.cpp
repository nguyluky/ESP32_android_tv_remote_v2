#include <Arduino.h>
#include <unity.h>
#include "pairing/PairingMessageManager.h"
#include "remote/RemoteMessageManager.h"
#include "utils.h"


void test_create_pairing_configuration() {
    uint8_t *buffer = pairingMessageManager.createPairingConfiguration();
    uint8_t e[] = {0x10, 0x08, 0x02, 0x10, 0xc8, 0x01, 0xf2, 0x01, 0x08, 0x0a, 0x04, 0x08, 0x03, 0x10, 0x06, 0x10, 0x01};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
    free(buffer);
}


void test_create_pairing_request() {
    uint8_t *buffer = pairingMessageManager.createPairingRequest("service_name");
    uint8_t e[] = {0x1E, 0x8, 0x2, 0x10, 0xC8, 0x1, 0x52, 0x17, 0xA, 0xC, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x5F, 0x6E, 0x61, 0x6D, 0x65, 0x12, 0x7, 0x47, 0x65, 0x6E, 0x65, 0x72, 0x69, 0x63};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
    free(buffer);
}

void test_create_pairing_option() {
    uint8_t *buffer = pairingMessageManager.createPairingOption();
    uint8_t e[] = {0x10, 0x8, 0x2, 0x10, 0xC8, 0x1, 0xA2, 0x1, 0x8, 0xA, 0x4, 0x8, 0x3, 0x10, 0x6, 0x18, 0x1};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
    free(buffer);
}

void test_create_pairing_secret() {
    uint8_t secret[] = {141, 244, 138, 57, 104, 74, 137, 202, 232, 210, 253, 115, 126, 84, 77, 127, 123, 81, 100, 50, 75, 102, 116, 211, 228, 246, 69, 128, 176, 105, 153, 86};
    uint8_t *buffer = pairingMessageManager.createPairingSecret(secret);
    uint8_t e[] = {0x2A, 0x8, 0x2, 0x10, 0xC8, 0x1, 0xC2, 0x2, 0x22, 0xA, 0x20, 0x8D, 0xF4, 0x8A, 0x39, 0x68, 0x4A, 0x89, 0xCA, 0xE8, 0xD2, 0xFD, 0x73, 0x7E, 0x54, 0x4D, 0x7F, 0x7B, 0x51, 0x64, 0x32, 0x4B, 0x66, 0x74, 0xD3, 0xE4, 0xF6, 0x45, 0x80, 0xB0, 0x69, 0x99, 0x56};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
    free(buffer);
}


// ====================================

void test_create_message_config() {
    uint8_t *buffer = remoteMessageManager.createMessageConfig("Mi TV", "Xiaomi");
    uint8_t e[] = {0x34, 0xA, 0x32, 0x8, 0xEE, 0x4, 0x12, 0x2D, 0xA, 0x5, 0x4D, 0x69, 0x20, 0x54, 0x56, 0x12, 0x6, 0x58, 0x69, 0x61, 0x6F, 0x6D, 0x69, 0x18, 0x1, 0x22, 0x1, 0x31, 0x2A, 0x10, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x74, 0x76, 0x2D, 0x72, 0x65, 0x6D, 0x6F, 0x74, 0x65, 0x32, 0x5, 0x31, 0x2E, 0x30, 0x2E, 0x30};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

void test_create_remote_set_active() {
    uint8_t *buffer = remoteMessageManager.createRemoteSetActive(true);
    uint8_t e[] = {0x4, 0x12, 0x2, 0x8, 0x1};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

void test_craate_remote_ping_response() {
    uint8_t *buffer = remoteMessageManager.createRemotePingResponse(1);
    uint8_t e[] = {0x4, 0x4A, 0x2, 0x8, 0x1};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

void test_create_remote_key_inject() {
    uint8_t *buffer = remoteMessageManager.createRemoteKeyInject(REMOTE__REMOTE_KEY_CODE__KEYCODE_POWER, REMOTE__REMOTE_DIRECTION__SHORT);
    uint8_t e[] = {0x6, 0x52, 0x4, 0x8, 0x1A, 0x10, 0x3};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

void test_create_remote_reset_preferred_audio_device() {
    uint8_t *buffer = remoteMessageManager.createRemoteResetPreferredAudioDevice();
    uint8_t e[] = {0x3, 0xEA, 0x3, 0x0};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

// function not use, so I comment it
// void test_create_remote_ime_key_inject() {
//     uint8_t *buffer = remoteMessageManager.createRemoteImeKeyInject("com.android.tv", REMOTE__REMOTE_TEXT_FIELD_STATUS__ACTIVE);
//     uint8_t e[] = {0x1A, 0x8, 0x2, 0x10, 0xC8, 0x1, 0x1A, 0x10, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2A, 0x8, 0x2, 0x10, 0x1, 0x18, 0x1};
//     printPacket(buffer, buffer[0] + 1);
//     TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
// }

void test_create_remote_remote_app_link_launch_request() {
    uint8_t *buffer = remoteMessageManager.createRemoteRemoteAppLinkLaunchRequest("com.android.tv");
    uint8_t e[] = {0x13, 0xD2, 0x5, 0x10, 0xA, 0xE, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x2E, 0x74, 0x76};
    printPacket(buffer, buffer[0] + 1);
    TEST_ASSERT_EQUAL_INT8_ARRAY(e, buffer, buffer[0] + 1);
}

void setup() {
    Serial.begin(115200);
    UNITY_BEGIN();
    RUN_TEST(test_create_pairing_configuration);
    RUN_TEST(test_create_pairing_request);
    RUN_TEST(test_create_pairing_option);
    RUN_TEST(test_create_pairing_secret);

    RUN_TEST(test_create_message_config);
    RUN_TEST(test_create_remote_set_active);
    RUN_TEST(test_craate_remote_ping_response);
    RUN_TEST(test_create_remote_key_inject);
    RUN_TEST(test_create_remote_reset_preferred_audio_device);
    // RUN_TEST(test_create_remote_ime_key_inject);
    RUN_TEST(test_create_remote_remote_app_link_launch_request);

    UNITY_END();
}

void loop() {
}