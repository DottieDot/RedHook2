#pragma once

#include "detour-hook.hpp"
#include "../rage/scrThread.hpp"

namespace rh2
{
    class CommandHook : public DetourHook
    {
      public:
        explicit CommandHook(const CommandHash native, rage::CommandHandler detour);

        void orig(rage::scrThread::Info* info);
    };
} // namespace rh2
