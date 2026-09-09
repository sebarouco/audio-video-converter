#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace infrastructure {
namespace config {

class AppConfig {
private:
    nlohmann::json config_;
    
    static AppConfig* instance_;
    
    AppConfig();
    void loadConfigFile(const std::string& configFile);
    
public:
    static AppConfig& getInstance();
    static void load(const std::string& configFile = "config/config.json");
    
    // Server configuration
    std::string getServerHost() const;
    int getServerPort() const;
    int getServerThreads() const;
    int getMaxConnections() const;
    
    // Database configuration
    std::string getDatabaseHost() const;
    int getDatabasePort() const;
    std::string getDatabaseName() const;
    std::string getDatabaseUser() const;
    std::string getDatabasePassword() const;
    int getDatabaseConnectionPoolSize() const;
    
    // Redis configuration
    std::string getRedisHost() const;
    int getRedisPort() const;
    std::string getRedisPassword() const;
    int getRedisDb() const;
    int getRedisConnectionPoolSize() const;
    
    // File storage configuration
    std::string getUploadDir() const;
    std::string getConvertedDir() const;
    std::string getTempDir() const;
    size_t getMaxFileSize() const;
    int getCleanupIntervalHours() const;
    int getFileRetentionHours() const;
    
    // FFmpeg configuration
    std::string getFFmpegPath() const;
    int getFFmpegThreads() const;
    int getFFmpegTimeoutSeconds() const;
    
    // JWT configuration
    std::string getJwtSecret() const;
    std::string getJwtAlgorithm() const;
    int getJwtExpirationHours() const;
    int getJwtRefreshExpirationDays() const;
    
    // Logging configuration
    std::string getLogLevel() const;
    std::string getLogFile() const;
    int getLogMaxSizeMb() const;
    int getLogMaxFiles() const;
    
    // WebSocket configuration
    bool isWebSocketEnabled() const;
    std::string getWebSocketPath() const;
    int getWebSocketPingIntervalSeconds() const;
};

} // namespace config
} // namespace infrastructure