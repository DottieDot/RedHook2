#include "invoker.hpp"

#include "crossmapping.hpp"
#include "../core/core.hpp"

#include <exception>
#include <Windows.h>

namespace rh2
{
    NativeHash            g_commandHash = 0x0;
    rage::scrThread::Info g_callInfo;

    void PatchVectorResults(rage::scrThread::Info* info)
    {
        static void (*fn)(rage::scrThread::Info * _this) = GetPatchVectorResults();

        fn(info);
    }

    void Invoker::_NativeInit(NativeHash hash)
    {
        g_commandHash = hash;
        g_callInfo.Reset();
    }

    void Invoker::_NativePush(u64 val)
    {
        g_callInfo.Push(val);
    }

    DECLSPEC_NOINLINE uintptr_t Invoker::_NativeCall()
    {
        if (!s_CommandHash)
        {
            throw std::exception("rage::g_CommandHash not initialized yet");
        }

        auto handler = GetCommandHandler(g_commandHash);

        if (handler)
        {
            __try
            {
                handler(&g_callInfo);
                PatchVectorResults(&g_callInfo);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        return reinterpret_cast<uintptr_t>(g_callInfo.GetResultPointer());
    }

    NativeHandler Invoker::GetCommandHandler(NativeHash command)
    {
        NativeHash currentHash = GetCurrentHash(command);
        if (!currentHash)
        {
            currentHash = command;
        }

        return rage::scrThread::GetCmdFromHash(currentHash);
    }

} // namespace rh2
