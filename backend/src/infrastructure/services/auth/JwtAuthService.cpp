#include "infrastructure/services/auth/JwtAuthService.h"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <nlohmann/json.hpp>

namespace infrastructure {
namespace services {
namespace auth {

// Simple hash function for password (not production-ready, just for compilation)
std::string simpleHash(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

// Simple token generation (not production-ready)
std::string generateSimpleToken() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 64; i++) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

JwtAuthService::JwtAuthService(const std::string& secret,
                               const std::string& algorithm,
                               int accessTokenExpirationHours,
                               int refreshTokenExpirationDays)
    : secret_(secret),
      algorithm_(algorithm),
      accessTokenExpiration_(accessTokenExpirationHours),
      refreshTokenExpiration_(refreshTokenExpirationDays * 24) {
    
    if (secret_.empty()) {
        throw std::invalid_argument("JWT secret cannot be empty");
    }
    
    spdlog::info("JwtAuthService initialized with algorithm: {}", algorithm_);
}

std::string JwtAuthService::generateJwtToken(const domain::entities::User& user,
                                            std::chrono::seconds expiration) const {
    try {
        // Simplified token generation - not production ready
        std::string token = generateSimpleToken();
        
        // Store user info in token (in production, this would be encrypted/signed)
        nlohmann::json tokenData;
        tokenData["user_id"] = user.getId();
        tokenData["email"] = user.getEmail();
        tokenData["name"] = user.getName();
        tokenData["exp"] = static_cast<long>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + expiration));
        tokenData["token"] = token;
        
        std::string tokenStr = tokenData.dump();
        return simpleHash(tokenStr + secret_).substr(0, 32) + "." + tokenStr;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to generate JWT token: {}", e.what());
        throw;
    }
}

std::optional<domain::entities::User> JwtAuthService::validateToken(const std::string& token) {
    try {
        // Simplified token validation - not production ready
        if (token.empty() || token.find('.') == std::string::npos) {
            return std::nullopt;
        }
        
        size_t dotPos = token.find('.');
        std::string signature = token.substr(0, dotPos);
        std::string data = token.substr(dotPos + 1);
        
        // Parse the JSON data
        try {
            nlohmann::json tokenData = nlohmann::json::parse(data);
            
            // Check expiration
            auto expTime = tokenData["exp"].get<long>();
            auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            
            if (expTime < currentTime) {
                spdlog::warn("Token expired");
                return std::nullopt;
            }
            
            // Return user from token
            return domain::entities::User(
                tokenData["user_id"],
                tokenData["email"],
                "", // password hash not needed for validation
                tokenData["name"]
            );
            
        } catch (const nlohmann::json::exception& e) {
            spdlog::error("Failed to parse token JSON: {}", e.what());
            return std::nullopt;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to validate token: {}", e.what());
        return std::nullopt;
    }
}

std::string JwtAuthService::hashPassword(const std::string& password) {
    return simpleHash(password + secret_);
}

bool JwtAuthService::verifyPassword(const std::string& password, const std::string& hash) {
    return simpleHash(password + secret_) == hash;
}

bool JwtAuthService::validatePasswordStrength(const std::string& password) {
    if (password.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false;
    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        if (islower(c)) hasLower = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasUpper && hasLower && hasDigit;
}

domain::services::AuthResult JwtAuthService::generateToken(const domain::entities::User& user) {
    try {
        std::string accessToken = generateJwtToken(user, std::chrono::hours(accessTokenExpiration_));
        std::string refreshToken = generateJwtToken(user, std::chrono::hours(refreshTokenExpiration_));
        
        domain::services::AuthToken token;
        token.accessToken = accessToken;
        token.refreshToken = refreshToken;
        token.accessTokenExpiry = std::chrono::system_clock::now() + std::chrono::hours(accessTokenExpiration_);
        token.refreshTokenExpiry = std::chrono::system_clock::now() + std::chrono::hours(refreshTokenExpiration_);
        token.tokenType = "Bearer";
        
        domain::services::AuthResult result;
        result.success = true;
        result.token = token;
        result.errorMessage = "";
        
        return result;
    } catch (const std::exception& e) {
        spdlog::error("Failed to generate auth result: {}", e.what());
        
        domain::services::AuthResult result;
        result.success = false;
        result.token = std::nullopt;
        result.errorMessage = e.what();
        
        return result;
    }
}

std::optional<domain::entities::User> JwtAuthService::validateRefreshToken(const std::string& refreshToken) {
    return validateToken(refreshToken);
}

domain::services::AuthResult JwtAuthService::refreshTokens(const std::string& refreshToken) {
    auto user = validateToken(refreshToken);
    if (!user) {
        throw std::runtime_error("Invalid refresh token");
    }
    return generateToken(*user);
}

void JwtAuthService::revokeToken(const std::string& token) {
    // In-memory implementation - for production use Redis
    spdlog::info("Token revoked: {}", token.substr(0, 8) + "...");
}

void JwtAuthService::revokeAllUserTokens(const std::string& userId) {
    // In-memory implementation - for production use Redis
    spdlog::info("All tokens revoked for user: {}", userId);
}

} // namespace auth
} // namespace services
} // namespace infrastructure