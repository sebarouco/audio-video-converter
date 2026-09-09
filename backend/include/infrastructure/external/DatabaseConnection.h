#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>
#include <mutex>
#include <vector>

namespace infrastructure {
namespace external {

class DatabaseConnection {
private:
    std::string dbPath_;
    sqlite3* db_;
    std::mutex connectionMutex_;
    
    void connect();
    void disconnect();
    bool isConnected() const;
    
public:
    explicit DatabaseConnection(const std::string& dbPath);
    ~DatabaseConnection();
    
    void executeQuery(const std::string& query);
    std::vector<std::vector<std::string>> executeQueryWithResult(const std::string& query);
    void reconnect();
    
    // For testing
    bool testConnection();
};

} // namespace external
} // namespace infrastructure