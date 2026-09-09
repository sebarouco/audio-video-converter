#pragma once

#include "application/interfaces/IUseCase.h"
#include "application/dto/UserDTO.h"
#include "domain/entities/User.h"
#include "domain/repositories/IUserRepository.h"
#include "domain/services/IAuthService.h"
#include <memory>
#include <string>

namespace application {
namespace usecases {
namespace auth {

struct RegisterUserRequest {
    std::string email;
    std::string password;
    std::string name;
};

struct RegisterUserResponse {
    dto::AuthResponse authResponse;
    bool success;
    std::string message;
};

class RegisterUserUseCase : public interfaces::IUseCase<RegisterUserRequest, RegisterUserResponse> {
private:
    std::shared_ptr<domain::repositories::IUserRepository> userRepository_;
    std::shared_ptr<domain::services::IAuthService> authService_;
    
    bool validateEmail(const std::string& email) const;
    bool validatePassword(const std::string& password) const;
    bool validateName(const std::string& name) const;
    std::string generateUserId() const;
    
public:
    RegisterUserUseCase(
        std::shared_ptr<domain::repositories::IUserRepository> userRepository,
        std::shared_ptr<domain::services::IAuthService> authService
    );
    
    RegisterUserResponse execute(const RegisterUserRequest& request) override;
    bool validateRequest(const RegisterUserRequest& request) const override;
    std::string getUseCaseName() const override { return "RegisterUserUseCase"; }
};

} // namespace auth
} // namespace usecases
} // namespace application