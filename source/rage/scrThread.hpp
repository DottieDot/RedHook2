#pragma once

#include "../types.hpp"

namespace rage
{
    using scrValue = rh2::u64;

    class scrThread
    {
      public:
        class Info
        {
          private:
            scrValue* m_return;
            rh2::u32  m_argCount = 0u;
            scrValue* m_args;
            rh2::u32  m_dataCount = 0u;
            alignas(rh2::u64) rh2::u8 m_vectorSpace[192]{};

            scrValue m_tempStack[32 /* max args */]{};

          public:
            Info() : m_return(m_tempStack), m_args(m_tempStack) {}

            template<typename T>
            inline T GetArgument(rh2::u32 index) const
            {
                return *reinterpret_cast<T*>(&m_args[index]);
            }

            template<typename T>
            inline void SetResult(rh2::u32 index, T value)
            {
                *reinterpret_cast<T*>(&m_args[index]) = value;
            }

            inline auto GetArgCount()
            {
                return m_argCount;
            }

            void Reset()
            {
                m_argCount  = 0;
                m_dataCount = 0;
            }

            inline rage::scrValue* GetResultPointer()
            {
                return m_return;
            }

            template<typename T>
            inline bool Push(T value)
            {
                if (sizeof(T) > sizeof(scrValue))
                {
                    return false;
                }
                else if (sizeof(T) < sizeof(scrValue))
                {
                    m_args[m_argCount] = 0ull;
                }

                m_args[m_argCount++] = *reinterpret_cast<scrValue*>(&value);

                return true
            }
        };
    };
}; // namespace rage
