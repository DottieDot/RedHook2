#include "fiber.hpp"

#include <Windows.h>

namespace rh2
{
    void Fiber::switchTo() const
    {
        if (m_fiber)
        {
            SwitchToFiber(m_fiber);
        }
    }

    void Fiber::remove()
    {
        if (m_fiber)
        {
            DeleteFiber(m_fiber);
        }
    }

    Fiber Fiber::CreateFiber(void (*startAddress)(void* pParam), void* pParam)
    {
        return ::CreateFiber(0, startAddress, pParam);
    }

    Fiber Fiber::CurrentFiber()
    {
        return GetCurrentFiber();
    }

    Fiber Fiber::ConvertThreadToFiber()
    {
        return ::ConvertThreadToFiber(nullptr);
    }
} // namespace rh2
