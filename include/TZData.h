#pragma once
#include <nlohmann/json.hpp>
#include <cstdint>

class ITZData {
public:
    virtual ~ITZData() = 0;
    virtual uint8_t getByteRepr() const = 0;
    [[nodiscard]] virtual bool isAPIKeyNeeded() const = 0;
    virtual void to_json(nlohmann::json& json) const = 0;
};
inline ITZData::~ITZData() {}

class PingData : public ITZData {
public:
    PingData();
    ~PingData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class TimezoneFromUserIDData : public ITZData {
private:
    uint64_t userId;
public:
    TimezoneFromUserIDData(uint64_t userId);
    ~TimezoneFromUserIDData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class TimezoneFromIPData : public ITZData {
private:
    std::string ip;
public:
    TimezoneFromIPData(const std::string& ip);
    ~TimezoneFromIPData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class UserIDUUIDLinkData : public ITZData {
private:
    uint64_t userId;
    std::string uuid;
private:
    UserIDUUIDLinkData(uint64_t userId, const std::string& uuid);
    ~UserIDUUIDLinkData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class TimezoneFromUUIDData : public ITZData {
private:
    std::string uuid;
public:
    TimezoneFromUUIDData(const std::string& uuid);
    ~TimezoneFromUUIDData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class IsLinkedData : public ITZData {
private:
    std::string uuid;
public:
    IsLinkedData(const std::string& uuid);
    ~IsLinkedData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class UserIdFromUUIDData : public ITZData {
private:
    std::string uuid;
public:
    UserIdFromUUIDData(const std::string& uuid);
    ~UserIdFromUUIDData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class UUIDFromUserIDData : public ITZData {
private:
    uint64_t userId;
public:
    UUIDFromUserIDData(uint64_t userId);
    ~UUIDFromUserIDData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};

class UpdateTimezoneData : public ITZData {
private:
    std::string uuid;
    std::string timezone;
public:
    UpdateTimezoneData(const std::string& uuid, const std::string& timezone);
    ~UpdateTimezoneData() override;

    uint8_t getByteRepr() const override;
    [[nodiscard]] bool isAPIKeyNeeded() const override;
    void to_json(nlohmann::json& json) const override;
};