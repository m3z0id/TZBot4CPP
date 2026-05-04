#pragma once
#include <algorithm>
#include <arpa/inet.h>
#include <filesystem>
#include <string>
#include <netdb.h>
#include <set>

inline std::string resolve(const std::string& hostname) {
    addrinfo hints{}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), nullptr, &hints, &res) != 0) return "";

    auto* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
    std::string ipStr;
    ipStr.resize(INET_ADDRSTRLEN);

    inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr.data(), INET_ADDRSTRLEN);
    freeaddrinfo(res);

    return ipStr;
}

inline bool isValidIP(const std::string& ip) {
    sockaddr_in sa{};
    return inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) == 1;
}

inline bool isValidUUID(const std::string& uuid) {
    if (uuid.length() != 36) return false;

    for (size_t i = 0; i < 36; ++i) {
        const char c = uuid[i];

        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (c != '-') return false;
        }

        else {
            if (!std::isxdigit(static_cast<unsigned char>(c))) return false;
        }
    }

    return true;
}

inline std::set<std::string> fetchValidTimezones() {
    const static std::string root = "/usr/share/zoneinfo";
    const static std::array<std::string, 3> blacklistedNames{"Etc", "posix", "right"};
    std::set<std::string> timezones = {};

    for (const auto& entry : std::filesystem::directory_iterator(root)) {
        if (!entry.is_directory() || std::ranges::find(blacklistedNames, entry.path().filename().string()) != blacklistedNames.end()) continue;
        for (const auto& p : std::filesystem::directory_iterator(entry.path())) {
            timezones.emplace(p.path().lexically_relative(root));
        }
    }

    return timezones;
}

inline bool isValidTimezone(const std::string& tz) {
    static std::set<std::string> validTimezones = fetchValidTimezones();
    return validTimezones.contains(tz);
}