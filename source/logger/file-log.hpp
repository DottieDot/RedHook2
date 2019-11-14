#pragma once

#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <deque>

#include "log.hpp"
#include "generic-logger.hpp"

namespace rh2::logging
{
    class FileLog : public Log
    {
        std::ofstream           m_file;
        std::thread             m_thread;
        std::atomic_bool        m_threadAlive;
        std::mutex              m_mutex;
        std::deque<std::string> m_queue;

      public:
        using Log::push;

        explicit FileLog(const std::filesystem::path& path);

        ~FileLog();

        void close() override;

      protected:
        void push(const std::string& message) override;

        void flush();
    };

    using GenericFileLogger = GenericLogger<FileLog>;
} // namespace rh2::logging
