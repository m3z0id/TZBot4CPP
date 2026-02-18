#include "TZBot/TZData.h"

#include "Helpers.h"

PingData::PingData() = default;
PingData::~PingData() = default;

uint8_t PingData::getByteRepr() const { return 0; }
bool PingData::isAPIKeyNeeded() const { return false; }
nlohmann::json PingData::toJson() const { return nlohmann::json::object(); }


TimezoneFromUserIDData::TimezoneFromUserIDData(const uint64_t userId) : userId(userId) {}
TimezoneFromUserIDData::~TimezoneFromUserIDData() = default;

uint8_t TimezoneFromUserIDData::getByteRepr() const { return 1; }
bool TimezoneFromUserIDData::isAPIKeyNeeded() const { return true; }
nlohmann::json TimezoneFromUserIDData::toJson() const {
    return {
        {"userId", userId}
    };
}


TimezoneFromIPData::TimezoneFromIPData(const std::string& ip) {
    if (!isValidIP(ip)) throw std::invalid_argument("Invalid IP");
    this->ip = ip;
}
TimezoneFromIPData::~TimezoneFromIPData() = default;

uint8_t TimezoneFromIPData::getByteRepr() const { return 2; }
bool TimezoneFromIPData::isAPIKeyNeeded() const { return true; }
nlohmann::json TimezoneFromIPData::toJson() const {
    return {
        {"ip", ip}
    };
}


UserIDUUIDLinkData::UserIDUUIDLinkData(const uint64_t userId, const std::string& uuid) : userId(userId) {
    if (!isValidUUID(uuid)) throw std::invalid_argument("Invalid UUID");
    this->uuid = uuid;
}
UserIDUUIDLinkData::~UserIDUUIDLinkData() = default;

uint8_t UserIDUUIDLinkData::getByteRepr() const { return 3; }
bool UserIDUUIDLinkData::isAPIKeyNeeded() const { return true; }
nlohmann::json UserIDUUIDLinkData::toJson() const {
    return {
        {"userId", userId},
        {"uuid", uuid}
    };
}


TimezoneFromUUIDData::TimezoneFromUUIDData(const std::string& uuid) {
    if (!isValidUUID(uuid)) throw std::invalid_argument("Invalid UUID");
    this->uuid = uuid;
}
TimezoneFromUUIDData::~TimezoneFromUUIDData() = default;
uint8_t TimezoneFromUUIDData::getByteRepr() const { return 4; }
bool TimezoneFromUUIDData::isAPIKeyNeeded() const { return true; }
nlohmann::json TimezoneFromUUIDData::toJson() const {
    return {
        {"uuid", uuid}
    };
}


IsLinkedData::IsLinkedData(const std::string& uuid) {
    if (!isValidUUID(uuid)) throw std::invalid_argument("Invalid UUID");
    this->uuid = uuid;
}
IsLinkedData::~IsLinkedData() = default;
uint8_t IsLinkedData::getByteRepr() const { return 5; }
bool IsLinkedData::isAPIKeyNeeded() const { return true; }
nlohmann::json IsLinkedData::toJson() const {
    return {
        {"uuid", uuid}
    };
}


UserIdFromUUIDData::UserIdFromUUIDData(const std::string& uuid) {
    if (!isValidUUID(uuid)) throw std::invalid_argument("Invalid UUID");
    this->uuid = uuid;
}
UserIdFromUUIDData::~UserIdFromUUIDData() = default;


uint8_t UserIdFromUUIDData::getByteRepr() const { return 6; }
bool UserIdFromUUIDData::isAPIKeyNeeded() const { return true; }
nlohmann::json UserIdFromUUIDData::toJson() const {
    return {
        {"uuid", uuid}
    };
}


UUIDFromUserIDData::UUIDFromUserIDData(uint64_t userId) : userId(userId) {}
UUIDFromUserIDData::~UUIDFromUserIDData() = default;

uint8_t UUIDFromUserIDData::getByteRepr() const { return 7; }
bool UUIDFromUserIDData::isAPIKeyNeeded() const { return true; }
nlohmann::json UUIDFromUserIDData::toJson() const {
    return {
        {"userId", userId}
    };
}


UpdateTimezoneData::UpdateTimezoneData(const std::string& uuid, const std::string& timezone) {
    if (!isValidUUID(uuid)) throw std::invalid_argument("Invalid UUID");
    if (!isValidTimezone(timezone)) throw std::invalid_argument("Invalid timezone");

    this->uuid = uuid;
    this->timezone = timezone;
}
UpdateTimezoneData::~UpdateTimezoneData() = default;

uint8_t UpdateTimezoneData::getByteRepr() const { return 8; }
bool UpdateTimezoneData::isAPIKeyNeeded() const { return true; }
nlohmann::json UpdateTimezoneData::toJson() const {
    return {
        {"uuid", uuid},
        {"timezone", timezone}
    };
}
