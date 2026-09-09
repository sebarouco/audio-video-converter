#pragma once

#include "application/interfaces/IUseCase.h"
#include "application/dto/UserDTO.h"
#include "domain/entities/User.h"
#include "domain/repositories/IUserRepository.h"
#include "domain/services/IAuthService.h"
#include <memory>
#include <string>
#include <optional>

namespace application {
namespace usecases {
namespace auth {

struct LoginUserRequest {
    std::string email;
    std::string password;
};

struct LoginUserResponse {
    dto::AuthResponse authResponse;
    bool success;
    std::string message;
};

class LoginUserUseCase : public interfaces::IUseCase<LoginUserRequest, LoginUserResponse> {
private:
    std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
    std::shared_ptr<domain::services::IAuthService> authService_;
    
    bool validateEmail(const std::string& email) const;
    bool validatePassword(const std::string& password) const;
    
public:
    LoginUserUseCase(
        std::shared_ptr<domain::repositories::IUserRepository> userRepository,
        std::shared_ptr<domain::services::IAuthService> authService
    );
    
    LoginUserResponse execute(const LoginUserRequest& request) override;
    bool validateRequest(const LoginUserRequest& request) const override;
    std::string getUseCaseName() const override { return "LoginUserUseCase"; }
};

} // namespace auth
} // namespace usecases
} // namespace application