#pragma once

#include "../types.hpp"
#include "detour-hook.hpp"
#include "../rage/scrThread.hpp"

namespace rh2::hooking
{
    class CommandHook : public DetourHook
    {
      public:
        explicit CommandHook(const NativeHash native, NativeHandler detour);

        void orig(rage::scrThread::Info* info);
    };
} // namespace rh2::hooking
