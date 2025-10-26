#pragma once

#include "common/protocol.h"

namespace dto_lobby {
#include "common/dto/macros.inl"

struct StartRequest {
    DECLARE_SERIALIZABLE(StartRequest)
    bool ready;
};

struct StartResponse {
    DECLARE_SERIALIZABLE(StartResponse)
};

#include "common/dto/macros_undef.inl"
}  // namespace dto_lobby
