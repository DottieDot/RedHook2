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

    void Fiber::remove() const
    {
        if (m_fiber)
        {
            DeleteFiber(m_fiber);
        }
    }

    Fiber rh2::Fiber::CreateFiber(StartRoutine startAddress, void* pParam)
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
