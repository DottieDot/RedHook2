#pragma once

#include "memory-location.hpp"

#include "../types.hpp"

namespace rh2
{
    MemoryLocation PatternScan(const char* pattern, const char* mask, hMod module = 0);

    MemoryLocation PatternScan(const char* pattern, const size_t len, const hMod module = 0);

    template<size_t len>
    MemoryLocation PatternScan(const char (&pattern)[len], const hMod module = 0)
    {
        return PatternScan(pattern, len, module);
    }

    namespace literals
    {
        MemoryLocation operator""_Scan(const char* pattern, const size_t len);
    } // namespace literals
} // namespace rh2
