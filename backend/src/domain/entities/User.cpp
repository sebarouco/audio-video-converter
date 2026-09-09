#include "domain/entities/User.h"
#include <regex>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace domain {
namespace entities {

User::User(std::string id, std::string email, std::string passwordHash, std::string name)
    : id_(std::move(id)), 
      email_(std::move(email)), 
      passwordHash_(std::move(passwordHash)), 
      name_(std::move(name)),
      createdAt_(std::chrono::system_clock::now()),
      updatedAt_(std::chrono::system_clock::now()) {
    
    if (!isValidEmail(email_)) {
        throw std::invalid_argument("Invalid email format");
    }
    if (!isValidPasswordHash(passwordHash_)) {
        throw std::invalid_argument("Invalid password hash");
    }
    if (!isValidName(name_)) {
        throw std::invalid_argument("Invalid name");
    }
}

void User::setEmail(const std::string& email) {
    if (!isValidEmail(email)) {
        throw std::invalid_argument("Invalid email format");
    }
    email_ = email;
    updateTimestamp();
}

void User::setPasswordHash(const std::string& passwordHash) {
    if (!isValidPasswordHash(passwordHash)) {
        throw std::invalid_argument("Invalid password hash");
    }
    passwordHash_ = passwordHash;
    updateTimestamp();
}

void User::setName(const std::string& name) {
    if (!isValidName(name)) {
        throw std::invalid_argument("Invalid name");
    }
    name_ = name;
    updateTimestamp();
}

void User::updateTimestamp() {
    updatedAt_ = std::chrono::system_clock::now();
}

bool User::isValidEmail(const std::string& email) const {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool User::isValidPasswordHash(const std::string& hash) const {
    // Basic validation - in production use proper password hashing validation
    return !hash.empty() && hash.length() >= 60; // bcrypt hash length
}

bool User::isValidName(const std::string& name) const {
    if (name.empty() || name.length() > 100) {
        return false;
    }
    // Name should only contain letters, spaces, and some special characters
    const std::regex pattern(R"(^[a-zA-Z\s\-']+$)");
    return std::regex_match(name, pattern);
}

} // namespace entities
} // namespace domain