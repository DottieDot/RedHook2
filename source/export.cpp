#include "export.hpp"

#include "core/core.hpp"
#include "hooking/input-hook.hpp"
#include "invoker/invoker.hpp"

void keyboardHandlerRegister(KeyboardHandler handler)
{
    rh2::hooking::input::AddCallback(handler);
}

void keyboardHandlerUnregister(KeyboardHandler handler)
{
    rh2::hooking::input::RemoveCallback(handler);
}

void scriptWait(DWORD time)
{
    rh2::ScriptWait(std::chrono::milliseconds(time));
}

void scriptRegister(HMODULE module, void (*LP_SCRIPT_MAIN)())
{
    rh2::ScriptRegister(module, rh2::Script(LP_SCRIPT_MAIN));
}

void scriptRegisterAdditionalThread(HMODULE module, void (*LP_SCRIPT_MAIN)())
{
    scriptRegister(module, LP_SCRIPT_MAIN);
}

void scriptUnregister(HMODULE module)
{
    rh2::ScriptUnregister(module);
}

void nativeInit(UINT64 hash)
{
    rh2::Invoker::NativeInit(hash);
}

void nativePush64(UINT64 val)
{
    rh2::Invoker::NativePush(val);
}

PUINT64 nativeCall()
{
    return reinterpret_cast<PUINT64>(rh2::Invoker::NativeCall());
}
