#pragma once
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

using Field = std::variant<std::string, int32_t, uint64_t>;

class TZResponse {
private:
    int code;
    Field message;
    TZResponse(const int code, const Field& message) : code(code), message(message) {}
public:
    TZResponse() = delete;
    static TZResponse fromJson(const nlohmann::json& json);

    [[nodiscard]] bool isSuccessful() const;
    [[nodiscard]] int getCode() const;

    [[nodiscard]] std::optional<std::string> getResponseAsString() const;
    [[nodiscard]] std::optional<uint32_t> getResponseAsInt() const;
    [[nodiscard]] std::optional<uint64_t> getResponseAsLong() const;
    [[nodiscard]] nlohmann::json toJson() const;
};
