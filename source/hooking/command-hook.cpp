#include "command-hook.hpp"

namespace rh2
{
    CommandHook::CommandHook(const CommandHash native, rage::CommandHandler detour) :
        DetourHook(commands::invoking::Invoker::GetCommandHandler(native), detour)
    {
    }

    void CommandHook::orig(rage::scrThread::Info* info)
    {
        DetourHook::orig<void>(info);
    }
} // namespace rh2
