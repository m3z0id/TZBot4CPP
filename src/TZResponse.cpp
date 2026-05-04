#include "TZBot/TZResponse.h"

#include "TZBot/Exceptions.h"

TZResponse::TZResponse(const nlohmann::json& json) : json(json) {
    if (!json.at("code").is_number_integer()) throw PacketParseException();
    code = json["code"];
}
TZResponse TZResponse::fromJson(const nlohmann::json& json) {
    return {json};
}
bool TZResponse::isSuccessful() const {
    return code == 200;
}
int TZResponse::getCode() const {
    return code;
}
std::optional<std::string> TZResponse::getResponseAsString() const {
    if (json.at("message").is_string()) return json.at("message");
    return std::nullopt;
}
std::optional<int32_t> TZResponse::getResponseAsInt() const {
    if (json.at("message").is_number_integer() &&
        json.at("message").get<int64_t>() >= std::numeric_limits<int32_t>::min() &&
        json.at("message").get<uint64_t>() <= std::numeric_limits<int32_t>::max()) return json.at("message").get<int32_t>();

    return std::nullopt;
}
std::optional<uint64_t> TZResponse::getResponseAsULong() const {
    if (json.at("message").is_number_integer() &&
        json.at("message").get<int64_t>() >= std::numeric_limits<uint64_t>::min() &&
        json.at("message").get<uint64_t>() <= std::numeric_limits<uint64_t>::max()) return json.at("message").get<uint64_t>();

    return std::nullopt;
}

nlohmann::json TZResponse::toJson() const {
    return json;
}
