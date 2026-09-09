#pragma once

#include <drogon/HttpController.h>
#include "application/usecases/auth/RegisterUserUseCase.h"
#include "application/usecases/auth/LoginUserUseCase.h"
#include "application/dto/UserDTO.h"

using namespace drogon;

namespace presentation {
namespace controllers {

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", Post);
    ADD_METHOD_TO(AuthController::loginUser, "/api/auth/login", Post);
    ADD_METHOD_TO(AuthController::refreshToken, "/api/auth/refresh", Post);
    ADD_METHOD_TO(AuthController::logoutUser, "/api/auth/logout", Post);
    METHOD_LIST_END
    
    void registerUser(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    
    void loginUser(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);
    
    void refreshToken(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);
    
    void logoutUser(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
    
private:
    std::shared_ptr<application::usecases::auth::RegisterUserUseCase> registerUseCase_;
    std::shared_ptr<application::usecases::auth::LoginUserUseCase> loginUseCase_;
    
    void extractUserFromRequest(const HttpRequestPtr& req, std::string& userId);
};

} // namespace controllers
} // namespace presentation