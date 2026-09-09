#pragma once

#include "domain/services/IAuthService.h"
#include <string>
#include <chrono>
#include <memory>

namespace infrastructure {
namespace services {
namespace auth {

class JwtAuthService : public domain::services::IAuthService {
private:
    std::string secret_;
    std::string algorithm_;
    std::chrono::hours accessTokenExpiration_;
    std::chrono::hours refreshTokenExpiration_;
    
    std::string generateJwtToken(const domain::entities::User& user, 
                                std::chrono::seconds expiration) const;
    
public:
    JwtAuthService(const std::string& secret,
                  const std::string& algorithm = "HS256",
                  int accessTokenExpirationHours = 24,
                  int refreshTokenExpirationDays = 7);
    
    ~JwtAuthService() override = default;
    
    domain::services::AuthResult generateToken(const domain::entities::User& user) override;
    std::optional<domain::entities::User> validateToken(const std::string& token) override;
    std::optional<domain::entities::User> validateRefreshToken(const std::string& refreshToken) override;
    domain::services::AuthResult refreshTokens(const std::string& refreshToken) override;
    void revokeToken(const std::string& token) override;
    void revokeAllUserTokens(const std::string& userId) override;
    
    std::string hashPassword(const std::string& password) override;
    bool verifyPassword(const std::string& password, 
                        const std::string& hash) override;
    bool validatePasswordStrength(const std::string& password) override;
};

} // namespace auth
} // namespace services
} // namespace infrastructure