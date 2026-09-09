#include "infrastructure/external/DatabaseConnection.h"
#include <spdlog/spdlog.h>

namespace infrastructure {
namespace external {

DatabaseConnection::DatabaseConnection(const std::string& dbPath)
    : dbPath_(dbPath), db_(nullptr) {
    connect();
}

DatabaseConnection::~DatabaseConnection() {
    disconnect();
}

void DatabaseConnection::connect() {
    try {
        int rc = sqlite3_open(dbPath_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::string errMsg = "Cannot open database: " + std::string(sqlite3_errmsg(db_));
            spdlog::error(errMsg);
            throw std::runtime_error(errMsg);
        }
        spdlog::info("Connected to SQLite database successfully: {}", dbPath_);
    } catch (const std::exception& e) {
        spdlog::error("Failed to connect to database: {}", e.what());
        throw;
    }
}

void DatabaseConnection::disconnect() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
        spdlog::info("Disconnected from database");
    }
}

bool DatabaseConnection::isConnected() const {
    return db_ != nullptr;
}

void DatabaseConnection::executeQuery(const std::string& query) {
    try {
        if (!isConnected()) {
            reconnect();
        }
        
        std::lock_guard<std::mutex> lock(connectionMutex_);
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            std::string error = "SQL error: " + std::string(errMsg);
            sqlite3_free(errMsg);
            spdlog::error(error);
            throw std::runtime_error(error);
        }
        
        spdlog::debug("Query executed successfully: {}", query.substr(0, 100));
    } catch (const std::exception& e) {
        spdlog::error("Failed to execute query: {}", e.what());
        throw;
    }
}

std::vector<std::vector<std::string>> DatabaseConnection::executeQueryWithResult(const std::string& query) {
    try {
        if (!isConnected()) {
            reconnect();
        }
        
        std::lock_guard<std::mutex> lock(connectionMutex_);
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            std::string error = "SQL prepare error: " + std::string(sqlite3_errmsg(db_));
            spdlog::error(error);
            throw std::runtime_error(error);
        }
        
        std::vector<std::vector<std::string>> results;
        int columnCount = sqlite3_column_count(stmt);
        
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            std::vector<std::string> row;
            for (int i = 0; i < columnCount; i++) {
                const unsigned char* text = sqlite3_column_text(stmt, i);
                row.push_back(text ? reinterpret_cast<const char*>(text) : "NULL");
            }
            results.push_back(row);
        }
        
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_DONE) {
            std::string error = "SQL step error: " + std::string(sqlite3_errmsg(db_));
            spdlog::error(error);
            throw std::runtime_error(error);
        }
        
        spdlog::debug("Query executed with result: {}", query.substr(0, 100));
        return results;
    } catch (const std::exception& e) {
        spdlog::error("Failed to execute query with result: {}", e.what());
        throw;
    }
}

void DatabaseConnection::reconnect() {
    spdlog::warn("Attempting to reconnect to database");
    disconnect();
    connect();
}

bool DatabaseConnection::testConnection() {
    try {
        if (!isConnected()) {
            connect();
        }
        
        std::lock_guard<std::mutex> lock(connectionMutex_);
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, "SELECT 1", -1, &stmt, nullptr);
        
        if (rc != SQLITE_OK) {
            spdlog::error("Database connection test failed: {}", sqlite3_errmsg(db_));
            return false;
        }
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_ROW) {
            spdlog::error("Database connection test failed: {}", sqlite3_errmsg(db_));
            return false;
        }
        
        spdlog::info("Database connection test successful");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Database connection test failed: {}", e.what());
        return false;
    }
}

} // namespace external
} // namespace infrastructure