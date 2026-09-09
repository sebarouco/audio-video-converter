#pragma once

#include <string>
#include <chrono>

namespace application {
namespace dto {

struct UserDTO {
    std::string id;
    std::string email;
    std::string name;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    
    UserDTO() = default;
    
    UserDTO(const std::string& id, const std::string& email, const std::string& name,
            std::chrono::system_clock::time_point createdAt,
            std::chrono::system_clock::time_point updatedAt)
        : id(id), email(email), name(name), 
          createdAt(createdAt), updatedAt(updatedAt) {}
};

struct RegisterUserRequest {
    std::string email;
    std::string password;
    std::string name;
};

struct LoginUserRequest {
    std::string email;
    std::string password;
};

struct AuthResponse {
    std::string accessToken;
    std::string refreshToken;
    std::string tokenType;
    std::chrono::system_clock::time_point expiresAt;
    UserDTO user;
};

} // namespace dto
} // namespace application