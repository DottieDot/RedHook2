#pragma once

#include <type_traits>

#include "log.hpp"

namespace rh2::logging
{
    template<typename LogType, typename = std::enable_if_t<std::is_base_of_v<Log, LogType>>>
    class GenericLogger : public LogType
    {
      public:
        using LogType::LogType;

        template<typename... Args>
        void log(const std::string& format_str, const Args&... args)
        {
            this->push("MESSAGE", fmt::format(format_str, args...));
        }

        template<typename... Args>
        void debug(const std::string& format_str, const Args&... args)
        {
            this->push("DEBUG", fmt::format(format_str, args...));
        }

        template<typename... Args>
        void warn(const std::string& format_str, const Args&... args)
        {
            this->push("WARN", fmt::format(format_str, args...));
        }

        template<typename... Args>
        void error(const std::string& format_str, const Args&... args)
        {
            this->push("ERROR", fmt::format(format_str, args...));
        }

        template<typename... Args>
        void fatal(const std::string& format_str, const Args&... args)
        {
            this->push("FATAL", fmt::format(format_str, args...));
        }
    };
} // namespace rh2::logging