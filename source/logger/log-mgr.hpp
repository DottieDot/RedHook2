#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <mutex>

#include "generic-logger.hpp"

namespace rh2::logging
{
    class LogMgr
    {
        static std::unordered_map<std::string_view, std::shared_ptr<Log>> sm_logs;
        static std::mutex                                                 sm_mutex;

      public:
        template<typename LogType,
                 typename... Args,
                 typename = std::enable_if_t<std::is_base_of_v<Log, LogType>>>
        static std::shared_ptr<LogType> CreateLog(const std::string_view name, Args&&... args)
        {
            auto log = std::make_shared<LogType>(std::forward<Args>(args)...);

            sm_logs.insert_or_assign(name, log);

            return log;
        }

        template<typename LogType, typename = std::enable_if_t<std::is_base_of_v<Log, LogType>>>
        static std::shared_ptr<LogType> Get(const std::string_view name)
        {
            std::lock_guard _(sm_mutex);

            return std::static_pointer_cast<LogType>(sm_logs.at(name));
        }

        static void Delete(const std::string_view name);

        static void DeleteAllLogs();
    };
} // namespace rh2::logging
