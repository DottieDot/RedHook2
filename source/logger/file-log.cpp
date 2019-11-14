#include "file-log.hpp"

namespace rh2::logging
{
    FileLog::FileLog(const std::filesystem::path& path) :
        m_file(path), m_threadAlive(true), m_thread([=]() mutable {
            using namespace std::chrono_literals;

            while (m_threadAlive)
            {
                flush();

                std::this_thread::sleep_for(1ms);
            }
        })
    {
        m_queue.push_back(helpers::CreateHeader());
    }

    FileLog::~FileLog()
    {
        close();
    }

    void FileLog::close()
    {
        m_threadAlive = false;

        if (m_thread.joinable())
        {
            m_thread.join();
        }
        else
        {
            flush();
        }
    }

    void FileLog::push(const std::string& message)
    {
        if (!m_threadAlive)
            return;

        std::lock_guard _(m_mutex);

        m_queue.push_back(message);
    }

    void FileLog::flush()
    {
        std::lock_guard _(m_mutex);

        for (const auto& message : m_queue)
        {
            m_file << message << '\n';
        }

        m_queue.clear();
        m_file << std::flush;

        if (!m_threadAlive)
        {
            m_file.close();
        }
    }
} // namespace rh2::logging
