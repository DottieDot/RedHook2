#include "invoker.hpp"

#include "crossmapping.hpp"
#include "../core/core.hpp"

#include <exception>
#include <Windows.h>
#include <unordered_map>

namespace rh2
{
    std::unordered_map<u64, NativeHandler> g_handlerCache;
    NativeHash                             g_commandHash = 0x0;
    rage::scrThread::Info                  g_callInfo;

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
        NativeHandler handler = GetCommandHandler(g_commandHash);

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
        auto& handler = g_handlerCache[command];
        if (handler == nullptr)
        {
            NativeHash currentHash = GetCurrentHash(command);
            if (!currentHash)
            {
                currentHash = command;
            }

            handler = rage::scrThread::GetCmdFromHash(currentHash);
        }

        return handler;
    }

} // namespace rh2
