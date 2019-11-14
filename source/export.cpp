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

void scriptWait(uint32_t time)
{
    rh2::ScriptWait(std::chrono::milliseconds(time));
}

void scriptRegister(void* module, void (*LP_SCRIPT_MAIN)())
{
    rh2::ScriptRegister(module, rh2::Script(LP_SCRIPT_MAIN));
}

void scriptRegisterAdditionalThread(void* module, void (*LP_SCRIPT_MAIN)())
{
    scriptRegister(module, LP_SCRIPT_MAIN);
}

void scriptUnregister(void* module)
{
    rh2::ScriptUnregister(module);
}

void nativeInit(uint64_t hash)
{
    rh2::Invoker::NativeInit(hash);
}

void nativePush64(uint64_t val)
{
    rh2::Invoker::NativePush(val);
}

uintptr_t nativeCall()
{
    return rh2::Invoker::NativeCall();
}
