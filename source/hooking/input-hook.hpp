#pragma once

#include <cstdint>

namespace rh2
{
    using KeyboardCallback = void (*)(uint32_t keyCode,
                                      uint16_t repeats,
                                      uint8_t  scanCode,
                                      bool     isExtended,
                                      bool     isWithAlt,
                                      bool     wasDownBefore,
                                      bool     isUpNow);

    bool InitializeHook();

    bool RemoveHook();

    void AddCallback(KeyboardCallback callback);

    void RemoveCallback(KeyboardCallback callback);
} // namespace rh2