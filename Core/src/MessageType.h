#ifndef MessageType_H
#define MessageType_H

#include <cstdint>

enum class MessageType : uint8_t {
    CONNECT = 1,
    DISCONNECT = 2,
    OBJECT = 3,
    CHAT = 4
};

#endif
