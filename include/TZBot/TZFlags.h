#pragma once
#include <cstdint>

enum class TZFlags : uint8_t {
    AES = 1,
    CHACHA20 = 1 << 1,
    MSGPACK = 1 << 3,
};

inline uint8_t operator|(TZFlags lhs, TZFlags rhs) {
    return static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs);
}

inline uint8_t operator|(uint8_t lhs, TZFlags rhs) {
    return lhs | static_cast<uint8_t>(rhs);
}

inline uint8_t operator|(TZFlags lhs, uint8_t rhs) {
    return static_cast<uint8_t>(lhs) | rhs;
}