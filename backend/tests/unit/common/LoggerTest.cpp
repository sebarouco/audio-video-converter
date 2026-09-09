#include <gtest/gtest.h>
#include "common/logging/Logger.h"
#include <filesystem>
#include <thread>
#include <chrono>

using namespace common::logging;

TEST(LoggerTest, InitializeWithoutError) {
    EXPECT_NO_THROW(Logger::init("test.log", "debug"));
}

TEST(LoggerTest, LogMethodsWork) {
    Logger::init("test.log", "debug");
    
    EXPECT_NO_THROW(Logger::info("Test info message"));
    EXPECT_NO_THROW(Logger::warn("Test warning message"));
    EXPECT_NO_THROW(Logger::error("Test error message"));
    EXPECT_NO_THROW(Logger::debug("Test debug message"));
}

TEST(LoggerTest, LogFileCreated) {
    std::string logFile = "test_logger.log";
    Logger::init(logFile, "debug");
    Logger::info("Test message");
    
    // Give it a moment to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_TRUE(std::filesystem::exists(logFile));
    
    // Cleanup
    if (std::filesystem::exists(logFile)) {
        std::filesystem::remove(logFile);
    }
}