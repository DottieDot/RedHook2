#pragma once

#include <cstdint>
#include <Windows.h>

#ifdef RH2_EXPORTS
#    define RH2_API __declspec(dllexport)
#else
#    define RH2_API __declspec(dllimport)
#endif

typedef void (*KeyboardHandler)(uint32_t keyCode,
                                uint16_t repeats,
                                uint8_t  scanCode,
                                bool     isExtended,
                                bool     isWithAlt,
                                bool     wasDownBefore,
                                bool     isUpNow);

RH2_API void keyboardHandlerRegister(KeyboardHandler handler);

RH2_API void keyboardHandlerUnregister(KeyboardHandler handler);

RH2_API void scriptWait(DWORD time);

RH2_API void scriptRegister(HMODULE module, void (*LP_SCRIPT_MAIN)());

RH2_API void scriptRegisterAdditionalThread(HMODULE module, void (*LP_SCRIPT_MAIN)());

RH2_API void scriptUnregister(HMODULE module);

RH2_API void nativeInit(UINT64 hash);

RH2_API void nativePush64(UINT64 val);

RH2_API PUINT64 nativeCall();

#ifdef __cplusplus
#    ifndef RH2_EXPORTS
namespace invoker
{
    static void NativeInit(uint64_t hash)
    {
        nativeInit(hash);
    }

    template<typename T>
    static void NativePush(T val)
    {
        nativePush64(*reinterpret_cast<uint64_t*>(&val));
    }

    template<typename T>
    static T NativeCall()
    {
        return *reinterpret_cast<T*>(nativeCall());
    }

    template<typename R, typename... Args>
    static R Invoke(NativeHash hash, const Args&... args)
    {
        NativeInit(hash);
        (NativePush(args), ...);
        return NativeCall<R>();
    }
} // namespace invoker
#    endif
#endif
