#pragma once

namespace rh2
{
    class Fiber
    {
      public:
        using StartRoutine = void(__cdecl*)(void* pParam);

      private:
        void* m_fiber = nullptr;

        Fiber(void* fiber) : m_fiber(fiber) {}

      public:
        Fiber() : m_fiber(nullptr) {}
        Fiber(const Fiber& fiber) : m_fiber(fiber.m_fiber) {}

        void switchTo() const;

        void remove() const;

        static Fiber CreateFiber(StartRoutine startAddress, void* pParam = nullptr);

        static Fiber CurrentFiber();

        static Fiber ConvertThreadToFiber();
    };
} // namespace rh2
