#pragma once

#include <Windows.h>
#include <cstdint>
#include <exception>

#include <MinHook/MinHook.h>

#include "../memory/memory-location.hpp"

namespace rh2
{
    class DetourHook
    {
        MemoryLocation m_target;
        void*          m_detour;
        void*          m_original;

      public:
        template<typename Ty>
        explicit DetourHook(const MemoryLocation& target, Ty detour) :
            m_target(target), m_detour(reinterpret_cast<void*>(detour))
        {
            const auto status = MH_CreateHook(m_target.as<void*>(), m_detour, &m_original);
            if (status != MH_OK)
            {
                throw std::exception("Failed to create hook!");
            }
        }

        ~DetourHook();

        bool enable() const;

        bool disable() const;

        template<typename Ty, typename... Args>
        Ty orig(Args... args)
        {
            return (static_cast<Ty (*)(Args...)>(m_original))(args...);
        }
    };

    bool DisableHooks();

    bool RemoveHooks();
} // namespace rh2
