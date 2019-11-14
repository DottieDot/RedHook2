#pragma once

#include <string>

#include "helpers.hpp"

namespace rh2::logging
{
    class Log
    {
      public:
        virtual ~Log() = default;

        virtual void close() = 0;

      protected:
        void push(const std::string& prefix, const std::string& message);

        virtual void push(const std::string& message) = 0;
    };
} // namespace rh2::logging