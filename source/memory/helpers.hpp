#pragma once

#include <cstdint>
#include <Windows.h>

#include "../types.hpp"

namespace rh2
{
    hMod GetHostModule();

    u64 GetBaseAddress(const hMod module = 0);

    u32 GetImageSize(const hMod module = 0);
} // namespace rh2
