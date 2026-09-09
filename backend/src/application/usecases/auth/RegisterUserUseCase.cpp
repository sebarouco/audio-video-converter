#include "application/usecases/auth/RegisterUserUseCase.h"
#include <random>
#include <regex>
#include <spdlog/spdlog.h>
#include <sstream>

namespace application {
namespace usecases {
namespace auth {

RegisterUserUseCase::RegisterUserUseCase(
    std::shared_ptr<domain::repositories::IUserRepository> userRepository,
    std::shared_ptr<domain::services::IAuthService> authService
) : userRepository_(userRepository), authService_(authService) {
    if (!userRepository_ || !authService_) {
        throw std::invalid_argument("Repository and auth service cannot be null");
    }
}

bool RegisterUserUseCase::validateEmail(const std::string& email) const {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool RegisterUserUseCase::validatePassword(const std::string& password) const {
    // Password should be at least 8 characters
    if (password.length() < 8) {
        return false;
    }
    
    // Should contain at least one uppercase letter
    if (!std::any_of(password.begin(), password.end(), ::isupper)) {
        return false;
    }
    
    // Should contain at least one lowercase letter
    if (!std::any_of(password.begin(), password.end(), ::islower)) {
        return false;
    }
    
    // Should contain at least one digit
    if (!std::any_of(password.begin(), password.end(), ::isdigit)) {
        return false;
    }
    
    return true;
}

bool RegisterUserUseCase::validateName(const std::string& name) const {
    if (name.empty() || name.length() > 100) {
        return false;
    }
    
    const std::regex pattern(R"(^[a-zA-Z\s\-']+$)");
    return std::regex_match(name, pattern);
}

std::string RegisterUserUseCase::generateUserId() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            ss << "-";
        }
        ss << std::hex << dis(gen);
    }
    
    return ss.str();
}

bool RegisterUserUseCase::validateRequest(const RegisterUserRequest& request) const {
    if (!validateEmail(request.email)) {
        spdlog::warn("Invalid email format: {}", request.email);
        return false;
    }
    
    if (!validatePassword(request.password)) {
        spdlog::warn("Password does not meet strength requirements");
        return false;
    }
    
    if (!validateName(request.name)) {
        spdlog::warn("Invalid name format: {}", request.name);
        return false;
    }
    
    return true;
}

RegisterUserResponse RegisterUserUseCase::execute(const RegisterUserRequest& request) {
    try {
        spdlog::info("Executing RegisterUserUseCase for email: {}", request.email);
        
        // Check if user already exists
        auto existingUser = userRepository_->findByEmail(request.email);
        if (existingUser.has_value()) {
            spdlog::warn("User with email {} already exists", request.email);
            return {
                .authResponse = {},
                .success = false,
                .message = "User with this email already exists"
            };
        }
        
        // Hash password
        std::string passwordHash = authService_->hashPassword(request.password);
        
        // Create new user
        std::string userId = generateUserId();
        domain::entities::User newUser(userId, request.email, passwordHash, request.name);
        
        // Save user
        userRepository_->save(newUser);
        
        // Generate auth tokens
        auto authResult = authService_->generateToken(newUser);
        
        if (!authResult.success) {
            spdlog::error("Failed to generate auth tokens for user: {}", userId);
            return {
                .authResponse = {},
                .success = false,
                .message = "Failed to generate authentication tokens"
            };
        }
        
        // Build response
        dto::UserDTO userDTO;
        userDTO.id = newUser.getId();
        userDTO.email = newUser.getEmail();
        userDTO.name = newUser.getName();
        userDTO.createdAt = newUser.getCreatedAt();
        userDTO.updatedAt = newUser.getUpdatedAt();
        
        dto::AuthResponse authResponse;
        if (authResult.token.has_value()) {
            authResponse.accessToken = authResult.token.value().accessToken;
            authResponse.refreshToken = authResult.token.value().refreshToken;
            authResponse.tokenType = authResult.token.value().tokenType;
            authResponse.expiresAt = authResult.token.value().accessTokenExpiry;
        }
        authResponse.user = userDTO;
        
        spdlog::info("User registered successfully: {}", userId);
        
        return {
            .authResponse = authResponse,
            .success = true,
            .message = "User registered successfully"
        };
        
    } catch (const std::exception& e) {
        spdlog::error("Error in RegisterUserUseCase: {}", e.what());
        return {
            .authResponse = {},
            .success = false,
            .message = std::string("Registration failed: ") + e.what()
        };
    }
}

} // namespace auth
} // namespace usecases
} // namespace application