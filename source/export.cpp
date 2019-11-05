#include "export.hpp"

#include "core/core.hpp"
#include "hooking/input-hook.hpp"

void keyboardHandlerRegister(KeyboardHandler handler)
{
    rh2::hooking::input::AddCallback(handler);
}

void keyboardHandlerUnregister(KeyboardHandler handler)
{
    rh2::hooking::input::AddCallback(handler);
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
    scriptUnregister(module);
}
