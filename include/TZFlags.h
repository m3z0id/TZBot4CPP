#pragma once
#include <cstdint>

enum class TZFlags : uint8_t {
    AES = 1,
    CHACHA20 = 1 << 1,
    GZIP = 1 << 2,
    MSGPACK = 1 << 3,
};
