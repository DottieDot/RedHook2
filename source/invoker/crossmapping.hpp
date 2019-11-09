#pragma once
#include "../types.hpp"

namespace rh2
{
    struct NativeTranslation
    {
        NativeHash m_origHash;
        NativeHash m_currentHash;
    };

    extern const NativeTranslation g_translationTable[6838];

    constexpr NativeHash GetOrigHash(NativeHash currentHash)
    {
        for (const auto& [orig, _new] : g_translationTable)
        {
            if (_new == currentHash)
            {
                return orig;
            }
        }

        return 0;
    }

    constexpr NativeHash GetCurrentHash(NativeHash origHash)
    {
        NativeHash res = 0;

        for (const auto& [orig, _new] : g_translationTable)
        {
            if (orig == origHash)
            {
                res = _new;
                break;
            }
        }

        return res;
    }
} // namespace rh2
