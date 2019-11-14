#pragma once

#include <type_traits>

#include "log.hpp"

namespace rh2::logging
{
    template<typename LogType, typename = std::enable_if_t<std::is_base_of_v<Log, LogType>>>
    class MetricLogger : public LogType
    {
      public:
        using LogType::LogType;

        template<typename... Args>
        void metric(const std::string& format_str, const Args&... args)
        {
            this->push("METRIC", fmt::format(format_str, args...));
        }

        template<typename... Args>
        void data(const std::string& format_str, const Args&... args)
        {
            this->push("DATA", fmt::format(format_str, args...));
        }
    };
} // namespace rh2::logging
