#include "pairing/PairingMessageManager.h"

bool decode_string_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    // Đọc dữ liệu từ stream
    std::string* str = reinterpret_cast<std::string*>(*arg);
    str->resize(stream->bytes_left);
    return pb_read(stream, reinterpret_cast<uint8_t*>(&(*str)[0]), stream->bytes_left);
}

bool encode_string_callback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
    const std::string* str = reinterpret_cast<const std::string*>(*arg);
    return pb_write(stream, reinterpret_cast<const uint8_t*>(str->c_str()), str->size());
}

PairingMessageManager::PairingMessageManager() {
    manufacturer = "ESP32";
    model = "Generic";
}

bool PairingMessageManager::createPairingRequest(Pairing__PairingMessage &message, const char *service_name) {
    message.pairing_request.service_name.funcs.encode = &encode_string_callback;
    message.pairing_request.service_name.funcs.decode = &decode_string_callback;
    message.pairing_request.service_name.arg = &service_name;

    message.pairing_request.client_name.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void * const *arg) -> bool {
        const char *client_name = (const char *)*arg;
        return pb_encode_tag_for_field(stream, field) && pb_encode_string(stream, (uint8_t *)client_name, strlen(client_name));
    };
    message.pairing_request.client_name.arg = &model;
    message.status = Pairing__PairingMessage_Status_STATUS_OK;
    message.protocol_version = 2;
    return true;
}

bool PairingMessageManager::createPairingOption(Pairing__PairingMessage &message) {
    message.pairing_option.preferred_role = pairing_RoleType_ROLE_TYPE_INPUT;
    message.pairing_option.input_encodings.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void * const *arg) -> bool {
        pairing_PairingEncoding *encodings = (pairing_PairingEncoding *)*arg;
        return pb_encode_tag_for_field(stream, field) && pb_encode_submessage(stream, pairing_PairingEncoding_fields, encodings);
    };
    pairing_PairingEncoding encoding = pairing_PairingEncoding_init_default;
    encoding.type = pairing_PairingEncoding_EncodingType_ENCODING_TYPE_HEXADECIMAL;
    encoding.symbol_length = 6;
    message.pairing_option.input_encodings.arg = &encoding;

    message.status = Pairing__PairingMessage_Status_STATUS_OK;
    message.protocol_version = 2;
    return true;
}

bool PairingMessageManager::createPairingConfiguration(Pairing__PairingMessage &message) {
    message.pairing_configuration.client_role = pairing_RoleType_ROLE_TYPE_INPUT;
    message.pairing_configuration.encoding.type = pairing_PairingEncoding_EncodingType_ENCODING_TYPE_HEXADECIMAL;
    message.pairing_configuration.encoding.symbol_length = 6;
    message.status = Pairing__PairingMessage_Status_STATUS_OK;
    message.protocol_version = 2;
    return true;
}

bool PairingMessageManager::createPairingSecret(Pairing__PairingMessage &message, const uint8_t *secret) {
    message.pairing_secret.secret.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void * const *arg) -> bool {
        uint8_t *secret = (uint8_t *)*arg;
        return pb_write(stream, secret, 32);
    };
    message.pairing_secret.secret.arg = &secret;
    message.status = Pairing__PairingMessage_Status_STATUS_OK;
    message.protocol_version = 2;
    return true;
}

bool PairingMessageManager::encodeMessage(Pairing__PairingMessage &message, uint8_t *buffer, size_t &size) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&stream, Pairing__PairingMessage_fields, &message)) {
        Serial.println("Encoding failed");
        return false;
    }
    size = stream.bytes_written;
    return true;
}

bool PairingMessageManager::decodeMessage(uint8_t *buffer, size_t size, Pairing__PairingMessage &message) {
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    if (!pb_decode(&stream, Pairing__PairingMessage_fields, &message)) {
        Serial.println("Decoding failed");
        return false;
    }
    return true;
}

// #include "pairing/PairingMessageManager.h"dd

// class PairingMessageManager {
// public:
//     PairingMessageManager() {
//         manufacturer = "ESP32";
//         model = "Generic";
//     }

//     bool createPairingRequest(Pairing__PairingMessage &message, const char *service_name) {
//         message.pairing_request.service_name.funcs.encode = &encode_string_callback;
//         message.pairing_request.service_name.funcs.decode = &decode_string_callback;
//         message.pairing_request.service_name.arg = &service_name;

//         message.pairing_request.client_name.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void * const *arg) -> bool {
//             const char *client_name = (const char *)*arg;
//             return pb_encode_tag_for_field(stream, field) && pb_encode_string(stream, (uint8_t *)client_name, strlen(client_name));
//         };
//         message.pairing_request.client_name.arg = &model;
//         message.status = Pairing__PairingMessage_Status_STATUS_OK;
//         message.protocol_version = 2;
//         return true;
//     }

//     bool createPairingOption(Pairing__PairingMessage &message) {
//         message.pairing_option.preferred_role = pairing_RoleType_ROLE_TYPE_INPUT;
//         message.pairing_option.input_encodings.funcs.encode = [](pb_ostream_t *stream, const pb_field_t *field, void * const *arg) -> bool {
//             pairing_PairingEncoding *encodings = (pairing_PairingEncoding *)*arg;
//             return pb_encode_tag_for_field(stream, field) && pb_encode_submessage(stream, pairing_PairingEncoding_fields, encodings);
//         };
//         pairing_PairingEncoding encoding = pairing_PairingEncoding_init_default;
//         encoding.type = pairing_PairingEncoding_EncodingType_ENCODING_TYPE_HEXADECIMAL;
//         encoding.symbol_length = 6;
//         message.pairing_option.input_encodings.arg = &encoding;

//         message.status = Pairing__PairingMessage_Status_STATUS_OK;
//         message.protocol_version = 2;
//         return true;
//     }

//     bool createPairingConfiguration(Pairing__PairingMessage &message) {
//         // message.which_message = PairingMessage_pairingConfiguration_tag;
//         // message.message.pairingConfiguration.clientRole = RoleType_ROLE_TYPE_INPUT;
//         // message.message.pairingConfiguration.encoding.type = PairingEncoding_EncodingType_ENCODING_TYPE_HEXADECIMAL;
//         // message.message.pairingConfiguration.encoding.symbolLength = 6;
//         // message.status = PairingMessage_Status_STATUS_OK;
//         // message.protocolVersion = 2;
//         message.pairing_configuration.client_role = pairing_RoleType_ROLE_TYPE_INPUT;
//         message.pairing_configuration.encoding.type = pairing_PairingEncoding_EncodingType_ENCODING_TYPE_HEXADECIMAL;
//         message.pairing_configuration.encoding.symbol_length = 6;
//         message.status = Pairing__PairingMessage_Status_STATUS_OK;
//         message.protocol_version = 2;
//         return true;
//     }

//     bool createPairingSecret(Pairing__PairingMessage &message, const char *secret) {
//         message.pairing_secret.secret.funcs.encode = &encode_string_callback;
//         message.pairing_secret.secret.funcs.decode = &decode_string_callback;
//         message.pairing_secret.secret.arg = &secret;
//         message.status = Pairing__PairingMessage_Status_STATUS_OK;
//         message.protocol_version = 2;
//         return true;
//     }

//     bool encodeMessage(Pairing__PairingMessage &message, uint8_t *buffer, size_t &size) {
//         pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
//         if (!pb_encode(&stream, Pairing__PairingMessage_fields, &message)) {
//             Serial.println("Encoding failed");
//             return false;
//         }
//         size = stream.bytes_written;
//         return true;
//     }

//     bool decodeMessage(uint8_t *buffer, size_t size, Pairing__PairingMessage &message) {
//         pb_istream_t stream = pb_istream_from_buffer(buffer, size);
//         if (!pb_decode(&stream, Pairing__PairingMessage_fields, &message)) {
//             Serial.println("Decoding failed");
//             return false;
//         }
//         return true;
//     }

// private:
//     const char *manufacturer;
//     const char *model;
// };

// PairingMessageManager pairingMessageManager;
