#include "detour-hook.hpp"

namespace rh2::hooking
{
    DetourHook::~DetourHook()
    {
        if (m_target.as<void*>())
        {
            MH_RemoveHook(m_target.as<void*>());
        }
    }

    bool DetourHook::enable() const
    {
        const auto status = MH_EnableHook(m_target.as<void*>());
        return status == MH_OK;
    }

    bool DetourHook::disable() const
    {
        const auto status = MH_DisableHook(m_target.as<void*>());
        return status == MH_OK;
    }

    bool DisableHooks()
    {
        const auto status = MH_DisableHook(MH_ALL_HOOKS);
        return status == MH_OK || status == MH_ERROR_NOT_CREATED ||
               status == MH_ERROR_NOT_INITIALIZED;
    }

    bool RemoveHooks()
    {
        const auto status = MH_RemoveHook(MH_ALL_HOOKS);
        return status == MH_OK || status == MH_ERROR_NOT_CREATED ||
               status == MH_ERROR_NOT_INITIALIZED;
    }
} // namespace rh2::hooking