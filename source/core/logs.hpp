#pragma once

#include "../logger/generic-file-logger.hpp"

#include <memory>

namespace rh2::logs
{
    extern std::shared_ptr<logging::GenericFileLogger> g_hLog;
} // namespace rh2::logs
