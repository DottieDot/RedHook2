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

        void init();

        void update();

        inline void wait(const std::chrono::high_resolution_clock::duration& duration)
        {
            m_wakeAt = std::chrono::high_resolution_clock::now() + duration;
            rh2::GetGameFiber().switchTo();
        }
    };
} // namespace rh2
