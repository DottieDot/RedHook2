#pragma once

#include "../util/fiber.hpp"
#include "../memory/memory-location.hpp"

namespace rh2
{
    bool Init();

    void Unload();

    Fiber GetGameFiber();

    MemoryLocation GetPatchVectorResults();

    MemoryLocation Get_s_CommandHash();

    MemoryLocation Get_rage__scrThread__GetCmdFromHash();
} // namespace rh2
