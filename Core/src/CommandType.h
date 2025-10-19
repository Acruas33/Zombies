#ifndef CommandType_H
#define CommandType_H

#include <cstdint>

enum class CommandType : uint8_t {
    SPAWNENEMY = 1,
    STARTGAME = 2
};

#endif