#include "helpers.hpp"

#include <ctime>

namespace rh2::logging::helpers
{
    std::string CreateTimestamp()
    {
        std::time_t t  = time(nullptr);
        std::tm     tm = { 0 };

        localtime_s(&tm, &t);

        const auto hours   = tm.tm_hour;
        const auto minutes = tm.tm_min;
        const auto seconds = tm.tm_sec;

        return fmt::format("{:0>2}:{:0>2}:{:0>2}", hours, minutes, seconds);
    }

    std::string CreateHeader()
    {
        static auto header = std::string()
                                 .append("RedHook2 (C)\n")
                                 .append(128, '-')
                                 .append("\nTIME     | TYPE    | MESSAGE\n")
                                 .append(128, '-');

        return header;
    }

    std::string CreatePrefixedMessage(const std::string& prefix, const std::string& message)
    {
        const auto timestamp = CreateTimestamp();

        return fmt::format("{} | {:<7} | {}", timestamp, prefix, message);
    }
} // namespace rh2::logging::helpers
