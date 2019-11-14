#include "log.hpp"

namespace rh2::logging
{
    void Log::push(const std::string& prefix, const std::string& message)
    {
        push(helpers::CreatePrefixedMessage(prefix, message));
    }
} // namespace rh2::logging
