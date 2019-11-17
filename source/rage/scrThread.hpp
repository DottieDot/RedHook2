#pragma once

#include "../types.hpp"

#include <cstring>

using NativeHandler                    = void (*)(void* params);
static inline rh2::u64** s_CommandHash = nullptr;

namespace rage
{
    using scrValue = rh2::u64;

#pragma pack(push, 1)
    class scrThread
    {
      public:
        static NativeHandler GetCmdFromHash(const rh2::NativeHash hash);

        class Info
        {
          private:
            scrValue* m_return;             // 0x00
            rh2::u32  m_argCount = 0u;      // 0x08
            char      _pad0x0C[0x4];        // 0x0C
            scrValue* m_args;               // 0x10
            rh2::u32  m_dataCount = 0u;     // 0x18
            char      _pad0x1C[0x4];        // 0x1C
            rh2::u8   m_vectorSpace[192]{}; // 0x20

            scrValue m_argStack[32 /* max args */]{};
            scrValue m_returnStack[16 /* should be enough, didn't check */]{};

          public:
            Info() : m_return(m_returnStack), m_args(m_argStack) {}

            template<typename T>
            inline T GetArgument(rh2::u32 index) const
            {
                return *reinterpret_cast<T*>(&m_args[index]);
            }

            template<typename T>
            inline void SetResult(rh2::u32 index, T value)
            {
                *reinterpret_cast<T*>(&m_return[index]) = value;
            }

            inline auto GetArgCount()
            {
                return m_argCount;
            }

            void Reset()
            {
                m_argCount  = 0;
                m_dataCount = 0;
                // MISC::_CREATE_VAR_STRING Doesn't always return... Thanks Rockstar!
                std::memset(m_returnStack, 0, sizeof(m_returnStack));
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

                return true;
            }
        };
    };
#pragma pack(pop)
}; // namespace rage
