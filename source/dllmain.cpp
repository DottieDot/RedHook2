// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

#include "core/core.hpp"

DWORD WINAPI ControlThread(const LPVOID param)
{
    if (!rh2::Init(param))
    {
        return 1;
    }

    return 0;
}

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD reasonForCall, const LPVOID lpReserved)
{
    switch (reasonForCall)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, ControlThread, static_cast<LPVOID>(hModule), 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH: rh2::Unload(); break;
    }

    return TRUE;
}
