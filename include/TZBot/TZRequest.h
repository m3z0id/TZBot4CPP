#pragma once
#include <string>

#include "TZData.h"

class TZRequest {
public:
    TZRequest(std::unique_ptr<ITZData> data);
    ~TZRequest();

    TZRequest(TZRequest&&) noexcept;
    TZRequest& operator=(TZRequest&&) noexcept;
    TZRequest(const TZRequest&) = delete;
    TZRequest& operator=(const TZRequest&) = delete;

    [[nodiscard]] nlohmann::json toJson() const;

    void setApiKey(const std::string& key);
    [[nodiscard]] uint8_t getRequestType() const;
private:
    std::string apiKey;
    std::unique_ptr<ITZData> data;
};