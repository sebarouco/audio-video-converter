#include "application/usecases/auth/LoginUserUseCase.h"
#include <regex>
#include <spdlog/spdlog.h>

namespace application {
namespace usecases {
namespace auth {

LoginUserUseCase::LoginUserUseCase(
    std::shared_ptr<domain::repositories::IUserRepository> userRepository,
    std::shared_ptr<domain::services::IAuthService> authService
) : userRepository_(userRepository), authService_(authService) {
    if (!userRepository_ || !authService_) {
        throw std::invalid_argument("Repository and auth service cannot be null");
    }
}

bool LoginUserUseCase::validateEmail(const std::string& email) const {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool LoginUserUseCase::validatePassword(const std::string& password) const {
    return !password.empty() && password.length() >= 1;
}

bool LoginUserUseCase::validateRequest(const LoginUserRequest& request) const {
    if (!validateEmail(request.email)) {
        spdlog::warn("Invalid email format: {}", request.email);
        return false;
    }
    
    if (!validatePassword(request.password)) {
        spdlog::warn("Empty password provided");
        return false;
    }
    
    return true;
}

LoginUserResponse LoginUserUseCase::execute(const LoginUserRequest& request) {
    try {
        spdlog::info("Executing LoginUserUseCase for email: {}", request.email);
        
        // Find user by email
        auto user = userRepository_->findByEmail(request.email);
        if (!user.has_value()) {
            spdlog::warn("User not found with email: {}", request.email);
            return {
                .authResponse = {},
                .success = false,
                .message = "Invalid email or password"
            };
        }
        
        // Verify password
        bool passwordValid = authService_->verifyPassword(request.password, user->getPasswordHash());
        if (!passwordValid) {
            spdlog::warn("Invalid password for user: {}", user->getId());
            return {
                .authResponse = {},
                .success = false,
                .message = "Invalid email or password"
            };
        }
        
        // Generate auth tokens
        auto authResult = authService_->generateToken(user.value());
        
        if (!authResult.success) {
            spdlog::error("Failed to generate auth tokens for user: {}", user->getId());
            return {
                .authResponse = {},
                .success = false,
                .message = "Failed to generate authentication tokens"
            };
        }
        
        // Build response
        dto::UserDTO userDTO;
        userDTO.id = user->getId();
        userDTO.email = user->getEmail();
        userDTO.name = user->getName();
        userDTO.createdAt = user->getCreatedAt();
        userDTO.updatedAt = user->getUpdatedAt();
        
        dto::AuthResponse authResponse;
        if (authResult.token.has_value()) {
            authResponse.accessToken = authResult.token.value().accessToken;
            authResponse.refreshToken = authResult.token.value().refreshToken;
            authResponse.tokenType = authResult.token.value().tokenType;
            authResponse.expiresAt = authResult.token.value().accessTokenExpiry;
        }
        authResponse.user = userDTO;
        
        spdlog::info("User logged in successfully: {}", user->getId());
        
        return {
            .authResponse = authResponse,
            .success = true,
            .message = "Login successful"
        };
        
    } catch (const std::exception& e) {
        spdlog::error("Error in LoginUserUseCase: {}", e.what());
        return {
            .authResponse = {},
            .success = false,
            .message = std::string("Login failed: ") + e.what()
        };
    }
}

} // namespace auth
} // namespace usecases
} // namespace application