#include "infrastructure/persistence/repositories/SqlUserRepository.h"
#include <spdlog/spdlog.h>

namespace infrastructure {
namespace persistence {
namespace repositories {

SqlUserRepository::SqlUserRepository(std::shared_ptr<external::DatabaseConnection> dbConnection)
    : dbConnection_(dbConnection) {
    
    if (!dbConnection_) {
        throw std::invalid_argument("Database connection cannot be null");
    }
    
    // Create users table if it doesn't exist
    try {
        std::string createTableQuery = R"(
            CREATE TABLE IF NOT EXISTS users (
                id TEXT PRIMARY KEY,
                email TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                name TEXT NOT NULL,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            );
            
            CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
        )";
        
        dbConnection_->executeQuery(createTableQuery);
        spdlog::info("Users table created/verified successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to create users table: {}", e.what());
        throw;
    }
}

domain::entities::User SqlUserRepository::modelToEntity(const models::UserModel& model) const {
    return domain::entities::User(
        model.id,
        model.email,
        model.passwordHash,
        model.name
    );
}

models::UserModel SqlUserRepository::entityToModel(const domain::entities::User& entity) const {
    return models::UserModel(
        entity.getId(),
        entity.getEmail(),
        entity.getPasswordHash(),
        entity.getName(),
        entity.getCreatedAt(),
        entity.getUpdatedAt()
    );
}

std::optional<domain::entities::User> SqlUserRepository::findById(const std::string& id) {
    try {
        std::string query = "SELECT * FROM users WHERE id = '" + id + "'";
        auto result = dbConnection_->executeQueryWithResult(query);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        models::UserModel model = models::UserModel::fromRow(result[0]);
        return modelToEntity(model);
        
    } catch (const std::exception& e) {
        spdlog::error("Error finding user by ID: {}", e.what());
        return std::nullopt;
    }
}

std::optional<domain::entities::User> SqlUserRepository::findByEmail(const std::string& email) {
    try {
        std::string query = "SELECT * FROM users WHERE email = '" + email + "'";
        auto result = dbConnection_->executeQueryWithResult(query);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        models::UserModel model = models::UserModel::fromRow(result[0]);
        return modelToEntity(model);
        
    } catch (const std::exception& e) {
        spdlog::error("Error finding user by email: {}", e.what());
        return std::nullopt;
    }
}

std::vector<domain::entities::User> SqlUserRepository::findAll(int limit, int offset) {
    try {
        std::string query = "SELECT * FROM users LIMIT " + std::to_string(limit) + 
                           " OFFSET " + std::to_string(offset);
        auto result = dbConnection_->executeQueryWithResult(query);
        
        std::vector<domain::entities::User> users;
        for (const auto& row : result) {
            models::UserModel model = models::UserModel::fromRow(row);
            users.push_back(modelToEntity(model));
        }
        
        return users;
        
    } catch (const std::exception& e) {
        spdlog::error("Error finding all users: {}", e.what());
        return {};
    }
}

void SqlUserRepository::save(const domain::entities::User& user) {
    try {
        models::UserModel model = entityToModel(user);
        std::string query = model.toInsertQuery();
        dbConnection_->executeQuery(query);
        
        spdlog::info("User saved successfully: {}", user.getId());
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving user: {}", e.what());
        throw;
    }
}

void SqlUserRepository::update(const domain::entities::User& user) {
    try {
        models::UserModel model = entityToModel(user);
        std::string query = model.toUpdateQuery();
        dbConnection_->executeQuery(query);
        
        spdlog::info("User updated successfully: {}", user.getId());
        
    } catch (const std::exception& e) {
        spdlog::error("Error updating user: {}", e.what());
        throw;
    }
}

void SqlUserRepository::deleteById(const std::string& id) {
    try {
        std::string query = "DELETE FROM users WHERE id = '" + id + "'";
        dbConnection_->executeQuery(query);
        
        spdlog::info("User deleted successfully: {}", id);
        
    } catch (const std::exception& e) {
        spdlog::error("Error deleting user: {}", e.what());
        throw;
    }
}

bool SqlUserRepository::existsById(const std::string& id) {
    return findById(id).has_value();
}

bool SqlUserRepository::existsByEmail(const std::string& email) {
    return findByEmail(email).has_value();
}

size_t SqlUserRepository::count() {
    try {
        std::string query = "SELECT COUNT(*) FROM users";
        auto result = dbConnection_->executeQueryWithResult(query);
        
        if (!result.empty() && !result[0].empty()) {
            return std::stoull(result[0][0]);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        spdlog::error("Error counting users: {}", e.what());
        return 0;
    }
}

} // namespace repositories
} // namespace persistence
} // namespace infrastructure