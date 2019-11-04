#pragma once

#include "../util/fiber.hpp"
#include "../core/core.hpp"

#include <chrono>

namespace rh2
{
    using ScriptFn = void (*)();

    class Script
    {
      private:
        ScriptFn                                       m_scriptFunction = nullptr;
        Fiber                                          m_fiber;
        std::chrono::high_resolution_clock::time_point m_wakeAt =
            std::chrono::high_resolution_clock::now();

      public:
        Script(ScriptFn scriptFunction) : m_scriptFunction(scriptFunction) {}

        inline void init()
        {
            m_fiber = Fiber::CreateFiber(reinterpret_cast<Fiber::StartRoutine>(m_scriptFunction),
                                         nullptr);
        }

        inline void update()
        {
            if (std::chrono::high_resolution_clock::now() < m_wakeAt)
                return;

            m_fiber.switchTo();
        }

        inline void wait(const std::chrono::high_resolution_clock::duration& duration)
        {
            m_wakeAt = std::chrono::high_resolution_clock::now() + duration;
            rh2::GetGameFiber().switchTo();
        }
    };
} // namespace rh2
