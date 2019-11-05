#include "script.hpp"

void rh2::Script::wait(const std::chrono::high_resolution_clock::duration& duration)
{
    m_wakeAt = std::chrono::high_resolution_clock::now() + duration;
    rh2::GetGameFiber().switchTo();
}
