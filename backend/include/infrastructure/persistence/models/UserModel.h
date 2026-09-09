#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace infrastructure {
namespace persistence {
namespace models {

class UserModel {
public:
    std::string id;
    std::string email;
    std::string passwordHash;
    std::string name;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    
    UserModel() = default;
    
    UserModel(const std::string& id, const std::string& email, 
              const std::string& passwordHash, const std::string& name,
              std::chrono::system_clock::time_point createdAt,
              std::chrono::system_clock::time_point updatedAt)
        : id(id), email(email), passwordHash(passwordHash), name(name),
          createdAt(createdAt), updatedAt(updatedAt) {}
    
    static UserModel fromRow(const std::vector<std::string>& row) {
        UserModel model;
        if (row.size() >= 6) {
            model.id = row[0];
            model.email = row[1];
            model.passwordHash = row[2];
            model.name = row[3];
            
            // Parse timestamps (simplified - using current time)
            model.createdAt = std::chrono::system_clock::now();
            model.updatedAt = std::chrono::system_clock::now();
        }
        return model;
    }
    
    std::string toInsertQuery() const {
        return std::string("INSERT INTO users (id, email, password_hash, name, created_at, updated_at) VALUES ('") +
               id + "', '" +
               email + "', '" +
               passwordHash + "', '" +
               name + "', " +
               "datetime('now'), " +
               "datetime('now'))";
    }
    
    std::string toUpdateQuery() const {
        return std::string("UPDATE users SET ") +
               "email = '" + email + "', " +
               "password_hash = '" + passwordHash + "', " +
               "name = '" + name + "', " +
               "updated_at = datetime('now') " +
               "WHERE id = '" + id + "'";
    }
};

} // namespace models
} // namespace persistence
} // namespace infrastructure