#include "common/logging/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <iostream>

namespace common {
namespace logging {

std::shared_ptr<spdlog::logger> Logger::logger_ = nullptr;
bool Logger::initialized_ = false;

void Logger::init(const std::string& logFile, const std::string& logLevel) {
    if (initialized_) {
        return;
    }
    
    try {
        // Create logs directory if it doesn't exist
        std::filesystem::path logPath(logFile);
        std::filesystem::create_directories(logPath.parent_path());
        
        // Create console sink
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::info);
        consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
        
        // Create file sink with rotation
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile, 1024 * 1024 * 100, 10); // 100MB per file, 10 files
        fileSink->set_level(spdlog::level::debug);
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] [%s:%#] %v");
        
        // Create logger with both sinks
        std::vector<spdlog::sink_ptr> sinks = {consoleSink, fileSink};
        logger_ = std::make_shared<spdlog::logger>("online_file_converter", sinks.begin(), sinks.end());
        
        // Set log level
        if (logLevel == "debug") {
            logger_->set_level(spdlog::level::debug);
        } else if (logLevel == "info") {
            logger_->set_level(spdlog::level::info);
        } else if (logLevel == "warn") {
            logger_->set_level(spdlog::level::warn);
        } else if (logLevel == "error") {
            logger_->set_level(spdlog::level::err);
        } else {
            logger_->set_level(spdlog::level::info);
        }
        
        // Set as default logger
        spdlog::set_default_logger(logger_);
        
        initialized_ = true;
        
        info("Logger initialized successfully");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

std::shared_ptr<spdlog::logger> Logger::getLogger() {
    if (!initialized_) {
        init();
    }
    return logger_;
}

void Logger::setLevel(const std::string& level) {
    if (!logger_) {
        return;
    }
    
    if (level == "debug") {
        logger_->set_level(spdlog::level::debug);
    } else if (level == "info") {
        logger_->set_level(spdlog::level::info);
    } else if (level == "warn") {
        logger_->set_level(spdlog::level::warn);
    } else if (level == "error") {
        logger_->set_level(spdlog::level::err);
    } else {
        logger_->set_level(spdlog::level::info);
    }
}

void Logger::info(const std::string& message) {
    if (logger_) {
        logger_->info(message);
    }
}

void Logger::warn(const std::string& message) {
    if (logger_) {
        logger_->warn(message);
    }
}

void Logger::error(const std::string& message) {
    if (logger_) {
        logger_->error(message);
    }
}

void Logger::debug(const std::string& message) {
    if (logger_) {
        logger_->debug(message);
    }
}

void Logger::critical(const std::string& message) {
    if (logger_) {
        logger_->critical(message);
    }
}

} // namespace logging
} // namespace common