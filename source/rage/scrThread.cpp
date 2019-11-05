#include "scrThread.hpp"

#include "../core/core.hpp"

namespace rage
{
    NativeHandler scrThread::GetCmdFromHash(const rh2::NativeHash hash)
    {
        static NativeHandler (*fn)(const rh2::NativeHash hash) =
            rh2::Get_rage__scrThread__GetCmdFromHash();

        return fn(hash);
    }
} // namespace rage
