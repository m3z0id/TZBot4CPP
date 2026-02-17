#include "../include/TZResponse.h"

TZResponse TZResponse::from_json(const nlohmann::json& json) {
    int code = json.at("code").get<int>();
    Field field;

    if (const nlohmann::json &msg = json.at("message"); msg.is_string()) field = msg.get<std::string>();
    else if (msg.is_number()) {
        if (auto temp = msg.get<uint64_t>(); temp > std::numeric_limits<int32_t>::max()) field = temp;
        else field = static_cast<int32_t>(temp);
    } else {
        throw std::runtime_error("invalid type");
    }

    return {code, field};
}
bool TZResponse::isSuccessful() const {
    return code == 200;
}
int TZResponse::getCode() const {
    return code;
}
std::optional<std::string> TZResponse::getResponseAsString() const {
    if (const auto p = std::get_if<std::string>(&message)) return *p;
    return std::nullopt;
}
std::optional<uint32_t> TZResponse::getResponseAsInt() const {
    if (const auto p = std::get_if<int32_t>(&message)) return *p;
    return std::nullopt;
}
std::optional<uint64_t> TZResponse::getResponseAsLong() const {
    if (const auto p = std::get_if<uint64_t>(&message)) return *p;
    return std::nullopt;
}