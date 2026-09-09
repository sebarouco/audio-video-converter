#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

namespace common {
namespace logging {

class Logger {
public:
    static void init(const std::string& logFile = "logs/application.log",
                    const std::string& logLevel = "info");
    
    static std::shared_ptr<spdlog::logger> getLogger();
    
    static void setLevel(const std::string& level);
    
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void debug(const std::string& message);
    static void critical(const std::string& message);
    
private:
    static std::shared_ptr<spdlog::logger> logger_;
    static bool initialized_;
};

} // namespace logging
} // namespace common