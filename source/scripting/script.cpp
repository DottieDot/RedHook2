#include "script.hpp"

namespace rh2
{
    void Script::init()
    {
        m_fiber =
            Fiber::CreateFiber(reinterpret_cast<Fiber::StartRoutine>(m_scriptFunction), nullptr);
    }

    void Script::update()
    {
        if (std::chrono::high_resolution_clock::now() < m_wakeAt)
        {
            return;
        }

        m_fiber.switchTo();
    }
} // namespace rh2
