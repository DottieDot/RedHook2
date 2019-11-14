#pragma once

#include <string>
#include <fmt/format.h>

namespace rh2::logging::helpers
{
    std::string CreateTimestamp();

    std::string CreateHeader();

    std::string CreatePrefixedMessage(const std::string& prefix, const std::string& message);
} // namespace rh2::logging::helpers