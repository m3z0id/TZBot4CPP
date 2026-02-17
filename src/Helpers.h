#pragma once
#include <algorithm>
#include <arpa/inet.h>
#include <filesystem>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <zlib.h>

inline std::vector<uint8_t> gzipCompress(const std::vector<uint8_t>& data, int level = Z_DEFAULT_COMPRESSION) {
    if (data.empty()) return {};

    z_stream stream{};
    stream.next_in = const_cast<Bytef*>(data.data());
    stream.avail_in = data.size();

    if (deflateInit2(&stream, level, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) throw std::runtime_error("deflateInit2 failed");

    std::vector<uint8_t> out{};
    out.resize(1024 * 8);
    int ret;

    do {
        if (stream.total_out >= out.size())  out.resize(out.size() * 2);

        stream.next_out = out.data() + stream.total_out;
        stream.avail_out = out.size() - stream.total_out;

        ret = deflate(&stream, Z_FINISH);

        if (ret == Z_STREAM_ERROR) {
            deflateEnd(&stream);
            throw std::runtime_error("deflate failed");
        }

    } while (ret != Z_STREAM_END);

    out.resize(stream.total_out);
    deflateEnd(&stream);

    return out;
}

inline std::vector<uint8_t> gzipDecompress(const std::vector<uint8_t>& compressed) {
    if (compressed.empty()) return {};

    z_stream stream{};
    stream.next_in = const_cast<Bytef*>(compressed.data());
    stream.avail_in = compressed.size();

    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK) throw std::runtime_error("inflateInit2 failed");

    std::vector<uint8_t> out{};
    out.resize(1024 * 8);

    int ret;
    do {
        if (stream.total_out >= out.size()) out.resize(out.size() * 2);

        stream.next_out = out.data() + stream.total_out;
        stream.avail_out = out.size() - stream.total_out;

        ret = inflate(&stream, Z_NO_FLUSH);

        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&stream);
            throw std::runtime_error("inflate failed");
        }

    } while (ret != Z_STREAM_END);

    out.resize(stream.total_out);
    inflateEnd(&stream);

    return out;
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

inline void fetchValidTimezones(std::vector<std::string>& timezones) {
    const static std::string root = "/usr/share/zoneinfo";
    const static std::array<std::string, 3> blacklistedNames{"Etc", "posix", "right"};
    timezones = {};

    for (const auto& entry : std::filesystem::directory_iterator(root)) {
        if (!entry.is_directory() || std::ranges::find(blacklistedNames, entry.path().filename().string()) != blacklistedNames.end()) continue;
        for (const auto& p : std::filesystem::directory_iterator(entry.path())) {
            timezones.emplace_back(p.path().lexically_relative(root));
        }
    }
}

inline bool isValidTimezone(const std::string& tz) {
    static std::vector<std::string> validTimezones = {};
    if (validTimezones.empty()) fetchValidTimezones(validTimezones);

    return std::ranges::find(validTimezones, tz) != validTimezones.end();
}