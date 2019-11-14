#pragma once

#include "generic-logger.hpp"
#include "file-log.hpp"

namespace rh2::logging
{
    using GenericFileLogger = logging::GenericLogger<logging::FileLog>;
}
