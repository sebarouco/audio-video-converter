#pragma once

#include <string>
#include <chrono>
#include <memory>

namespace domain {
namespace entities {

class User {
private:
    std::string id_;
    std::string email_;
    std::string passwordHash_;
    std::string name_;
    std::chrono::system_clock::time_point createdAt_;
    std::chrono::system_clock::time_point updatedAt_;

public:
    User(std::string id, std::string email, std::string passwordHash, std::string name);
    
    // Getters
    std::string getId() const { return id_; }
    std::string getEmail() const { return email_; }
    std::string getPasswordHash() const { return passwordHash_; }
    std::string getName() const { return name_; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt_; }
    std::chrono::system_clock::time_point getUpdatedAt() const { return updatedAt_; }
    
    // Setters with validation
    void setEmail(const std::string& email);
    void setPasswordHash(const std::string& passwordHash);
    void setName(const std::string& name);
    void updateTimestamp();
    
    // Validation
    bool isValidEmail(const std::string& email) const;
    bool isValidPasswordHash(const std::string& hash) const;
    bool isValidName(const std::string& name) const;
};

} // namespace entities
} // namespace domain