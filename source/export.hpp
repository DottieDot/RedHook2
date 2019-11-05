#pragma once

#include <cstdint>
#include <Windows.h>

#ifdef RH2_EXPORTS
#    define RH2_API __declspec(dllexport)
#else
#    define RH2_API __declspec(dllimport)
#endif

using KeyboardHandler = void (*)(uint32_t keyCode,
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
