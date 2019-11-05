#include "command-hook.hpp"

#include "../invoker/invoker.hpp"

namespace rh2::hooking
{
    CommandHook::CommandHook(const NativeHash native, NativeHandler detour) :
        DetourHook(Invoker::GetCommandHandler(native), detour)
    {
    }

    void CommandHook::orig(rage::scrThread::Info* info)
    {
        DetourHook::orig<void>(info);
    }
} // namespace rh2::hooking
