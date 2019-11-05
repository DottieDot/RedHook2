#pragma once
#include "../types.hpp"

namespace rh2
{
    constexpr NativeHash GetOrigHash(NativeHash currentHash);

    constexpr NativeHash GetCurrentHashCexpr(NativeHash origHash);

    NativeHash GetCurrentHash(NativeHash origHash);
} // namespace rh2
