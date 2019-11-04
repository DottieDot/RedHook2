#include "core.hpp"

namespace rh2
{
    Fiber g_gameFiber;

    void Init() {}

    Fiber GetGameFiber()
    {
        return g_gameFiber;
    }
} // namespace rh2
