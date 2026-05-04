#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

class TZResponse {
private:
    int code;
    nlohmann::json json;
    TZResponse(const nlohmann::json& json);
public:
    TZResponse() = delete;
    static TZResponse fromJson(const nlohmann::json& json);

    [[nodiscard]] bool isSuccessful() const;
    [[nodiscard]] int getCode() const;

    [[nodiscard]] std::optional<std::string> getResponseAsString() const;
    [[nodiscard]] std::optional<int32_t> getResponseAsInt() const;
    [[nodiscard]] std::optional<uint64_t> getResponseAsULong() const;
    [[nodiscard]] nlohmann::json toJson() const;
};
