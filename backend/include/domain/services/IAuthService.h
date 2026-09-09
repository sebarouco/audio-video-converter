#pragma once

#include <string>
#include <optional>
#include <chrono>
#include "domain/entities/User.h"

namespace domain {
namespace services {

struct AuthToken {
    std::string accessToken;
    std::string refreshToken;
    std::chrono::system_clock::time_point accessTokenExpiry;
    std::chrono::system_clock::time_point refreshTokenExpiry;
    std::string tokenType;
};

struct AuthResult {
    bool success;
    std::optional<AuthToken> token;
    std::string errorMessage;
};

class IAuthService {
public:
    virtual ~IAuthService() = default;
    
    virtual AuthResult generateToken(const entities::User& user) = 0;
    virtual std::optional<entities::User> validateToken(const std::string& token) = 0;
    virtual std::optional<entities::User> validateRefreshToken(const std::string& refreshToken) = 0;
    virtual AuthResult refreshTokens(const std::string& refreshToken) = 0;
    virtual void revokeToken(const std::string& token) = 0;
    virtual void revokeAllUserTokens(const std::string& userId) = 0;
    
    virtual std::string hashPassword(const std::string& password) = 0;
    virtual bool verifyPassword(const std::string& password, 
                                const std::string& hash) = 0;
    virtual bool validatePasswordStrength(const std::string& password) = 0;
};

} // namespace services
} // namespace domain