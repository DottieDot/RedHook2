#include "log-mgr.hpp"

namespace rh2::logging
{
    std::unordered_map<std::string_view, std::shared_ptr<Log>> LogMgr::sm_logs;
    std::mutex                                                 LogMgr::sm_mutex;

    void LogMgr::Delete(const std::string_view name)
    {
        std::lock_guard _(sm_mutex);

        if (auto log = sm_logs.find(name); log != sm_logs.end() && log->second)
        {
            log->second->close();
            sm_logs.erase(log);
            return;
        }
    }

    void LogMgr::DeleteAllLogs()
    {
        std::lock_guard _(sm_mutex);

        for (const auto& [name, log] : sm_logs)
        {
            log->close();
        }

        sm_logs.clear();
    }
} // namespace rh2::logging
