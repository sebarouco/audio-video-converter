#include "infrastructure/config/AppConfig.h"
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

namespace infrastructure {
namespace config {

AppConfig* AppConfig::instance_ = nullptr;

AppConfig::AppConfig() {
    // Default constructor
}

AppConfig& AppConfig::getInstance() {
    if (!instance_) {
        instance_ = new AppConfig();
    }
    return *instance_;
}

void AppConfig::load(const std::string& configFile) {
    getInstance().loadConfigFile(configFile);
}

void AppConfig::loadConfigFile(const std::string& configFile) {
    try {
        std::ifstream configStream(configFile);
        if (!configStream.is_open()) {
            throw std::runtime_error("Failed to open config file: " + configFile);
        }
        
        configStream >> config_;
        spdlog::info("Configuration loaded from: {}", configFile);
    } catch (const std::exception& e) {
        spdlog::error("Failed to load configuration: {}", e.what());
        throw;
    }
}

// Server configuration
std::string AppConfig::getServerHost() const {
    return config_.value("server/host", "0.0.0.0");
}

int AppConfig::getServerPort() const {
    return config_.value("server/port", 8080);
}

int AppConfig::getServerThreads() const {
    return config_.value("server/threads", 4);
}

int AppConfig::getMaxConnections() const {
    return config_.value("server/max_connections", 1000);
}

// Database configuration
std::string AppConfig::getDatabaseHost() const {
    return config_.value("database/host", "localhost");
}

int AppConfig::getDatabasePort() const {
    return config_.value("database/port", 5432);
}

std::string AppConfig::getDatabaseName() const {
    return config_.value("database/name", "online_file_converter");
}

std::string AppConfig::getDatabaseUser() const {
    return config_.value("database/user", "converter_user");
}

std::string AppConfig::getDatabasePassword() const {
    return config_.value("database/password", "converter_password");
}

int AppConfig::getDatabaseConnectionPoolSize() const {
    return config_.value("database/connection_pool_size", 10);
}

// Redis configuration
std::string AppConfig::getRedisHost() const {
    return config_.value("redis/host", "localhost");
}

int AppConfig::getRedisPort() const {
    return config_.value("redis/port", 6379);
}

std::string AppConfig::getRedisPassword() const {
    return config_.value("redis/password", "");
}

int AppConfig::getRedisDb() const {
    return config_.value("redis/db", 0);
}

int AppConfig::getRedisConnectionPoolSize() const {
    return config_.value("redis/connection_pool_size", 5);
}

// File storage configuration
std::string AppConfig::getUploadDir() const {
    return config_.value("file_storage/upload_dir", "./data/uploads");
}

std::string AppConfig::getConvertedDir() const {
    return config_.value("file_storage/converted_dir", "./data/converted");
}

std::string AppConfig::getTempDir() const {
    return config_.value("file_storage/temp_dir", "./data/temp");
}

size_t AppConfig::getMaxFileSize() const {
    return config_.value("file_storage/max_file_size", 536870912); // 512MB
}

int AppConfig::getCleanupIntervalHours() const {
    return config_.value("file_storage/cleanup_interval_hours", 24);
}

int AppConfig::getFileRetentionHours() const {
    return config_.value("file_storage/file_retention_hours", 48);
}

// FFmpeg configuration
std::string AppConfig::getFFmpegPath() const {
    return config_.value("ffmpeg/path", "/usr/bin/ffmpeg");
}

int AppConfig::getFFmpegThreads() const {
    return config_.value("ffmpeg/threads", 2);
}

int AppConfig::getFFmpegTimeoutSeconds() const {
    return config_.value("ffmpeg/timeout_seconds", 3600);
}

// JWT configuration
std::string AppConfig::getJwtSecret() const {
    return config_.value("jwt/secret", "your-secret-key-change-in-production");
}

std::string AppConfig::getJwtAlgorithm() const {
    return config_.value("jwt/algorithm", "HS256");
}

int AppConfig::getJwtExpirationHours() const {
    return config_.value("jwt/expiration_hours", 24);
}

int AppConfig::getJwtRefreshExpirationDays() const {
    return config_.value("jwt/refresh_expiration_days", 7);
}

// Logging configuration
std::string AppConfig::getLogLevel() const {
    return config_.value("logging/level", "info");
}

std::string AppConfig::getLogFile() const {
    return config_.value("logging/file", "./logs/application.log");
}

int AppConfig::getLogMaxSizeMb() const {
    return config_.value("logging/max_size_mb", 100);
}

int AppConfig::getLogMaxFiles() const {
    return config_.value("logging/max_files", 10);
}

// WebSocket configuration
bool AppConfig::isWebSocketEnabled() const {
    return config_.value("websocket/enabled", true);
}

std::string AppConfig::getWebSocketPath() const {
    return config_.value("websocket/path", "/ws");
}

int AppConfig::getWebSocketPingIntervalSeconds() const {
    return config_.value("websocket/ping_interval_seconds", 30);
}

} // namespace config
} // namespace infrastructure