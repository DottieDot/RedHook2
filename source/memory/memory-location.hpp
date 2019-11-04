#pragma once

#include <cstdint>
#include <Windows.h>
#include <vector>
#include <cmath>

namespace rh2
{
    class MemoryLocation
    {
      private:
        static std::vector<struct ModifiedMemoryData> sm_modifiedLocations;

        uintptr_t m_location;

        bool
            protect(const size_t size, const uint32_t newProtect, const uint32_t* oldProtect) const;

      public:
        MemoryLocation() : m_location(0) {}

        template<typename T>
        MemoryLocation(T location) : m_location(*reinterpret_cast<uintptr_t*>(&location))
        {
        }

        template<typename T>
        T as() const
        {
            return *reinterpret_cast<T*>(const_cast<uintptr_t*>(&m_location));
        }

        template<typename T>
        T get() const
        {
            return *as<T*>();
        }

        template<typename T>
        MemoryLocation add(const T offset) const
        {
            return MemoryLocation(m_location + offset);
        }

        template<typename T>
        MemoryLocation sub(const T offset) const
        {
            return MemoryLocation(m_location - offset);
        }

        template<typename T>
        MemoryLocation rip(const T rip) const
        {
            uint16_t shift = (8 - rip) * 8;

            return add(rip).add(get<int64_t>() << shift >> shift);
        }

        const MemoryLocation& set(const uint8_t value, const size_t bytes) const
        {
            saveBytes(bytes);
            memset(as<void*>(), value, bytes);
            return *this;
        }

        bool set_vp(const uint8_t value, const size_t bytes) const
        {
            uint32_t oldProtect = 0;

            if (protect(bytes, PAGE_EXECUTE_READWRITE, &oldProtect))
            {
                set(value, bytes);
                protect(bytes, oldProtect, nullptr);

                return true;
            }

            return false;
        }

        template<typename T>
        const MemoryLocation& write(const T value) const
        {
            saveBytes(sizeof(T));
            *as<T*>() = value;
            return *this;
        }

        template<typename T>
        bool write_vp(const T value) const
        {
            uint32_t oldProtect = 0;
            size_t   size       = sizeof(value);

            if (protect(size, PAGE_EXECUTE_READWRITE, &oldProtect))
            {
                write(value);
                protect(size, oldProtect, nullptr);

                return true;
            }

            return false;
        }

        const MemoryLocation& nop(const size_t bytes) const
        {
            set(0x90, bytes);
            return *this;
        }

        const MemoryLocation& ret(uint16_t stackSize = 0) const
        {
            if (stackSize)
            {
                write<BYTE>(0xC2).add(1).write(stackSize);
            }
            else
            {
                write<BYTE>(0xC3);
            }

            return *this;
        }

        bool nop_vp(const size_t bytes) const
        {
            return set_vp(0x90, bytes);
        }

        bool ret_vp(uint16_t stackSize = 0) const
        {
            if (stackSize)
            {
                return write_vp<BYTE>(0xC2) && add(1).write_vp(stackSize);
            }
            else
            {
                return write_vp<BYTE>(0xC3);
            }
        }

        MemoryLocation get_call() const
        {
            return add(1).rip(4);
        }

        MemoryLocation get_jmp() const
        {
            return get_call();
        }

        MemoryLocation get_lea() const
        {
            return add(3).rip(4);
        }

        void saveBytes(const size_t numBytes) const;

        void restore() const;

        template<typename T>
        operator T()
        {
            return as<T>();
        }

        static void RestoreAllModifiedBytes();
    };

    struct ModifiedMemoryData
    {
        MemoryLocation loc              = 0;
        size_t         numModifiedBytes = 0;
        uint8_t*       origBytes        = nullptr;
    };
} // namespace rh2
