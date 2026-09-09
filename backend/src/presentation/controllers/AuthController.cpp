#include "presentation/controllers/AuthController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <spdlog/spdlog.h>

using namespace drogon;

namespace presentation {
namespace controllers {

void AuthController::registerUser(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        spdlog::info("POST /api/auth/register");
        
        // Parse request body
        auto json = req->getJsonObject();
        if (!json) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Invalid request body";
            errorJson["error"] = "JSON body required";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k400BadRequest);
            callback(response);
            return;
        }
        
        std::string email = (*json)["email"].asString();
        std::string password = (*json)["password"].asString();
        std::string name = (*json)["name"].asString();
        
        // Create use case request
        application::usecases::auth::RegisterUserRequest request;
        request.email = email;
        request.password = password;
        request.name = name;
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value userJson;
        userJson["id"] = "mock_user_id";
        userJson["email"] = email;
        userJson["name"] = name;
        
        Json::Value dataJson;
        dataJson["accessToken"] = "mock_access_token";
        dataJson["refreshToken"] = "mock_refresh_token";
        dataJson["tokenType"] = "Bearer";
        dataJson["user"] = userJson;
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "User registered successfully (mock)";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in registerUser: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Registration failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void AuthController::loginUser(const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        spdlog::info("POST /api/auth/login");
        
        // Parse request body
        auto json = req->getJsonObject();
        if (!json) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Invalid request body";
            errorJson["error"] = "JSON body required";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k400BadRequest);
            callback(response);
            return;
        }
        
        std::string email = (*json)["email"].asString();
        std::string password = (*json)["password"].asString();
        
        // Create use case request
        application::usecases::auth::LoginUserRequest request;
        request.email = email;
        request.password = password;
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value userJson;
        userJson["id"] = "mock_user_id";
        userJson["email"] = email;
        userJson["name"] = "Mock User";
        
        Json::Value dataJson;
        dataJson["accessToken"] = "mock_access_token";
        dataJson["refreshToken"] = "mock_refresh_token";
        dataJson["tokenType"] = "Bearer";
        dataJson["user"] = userJson;
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Login successful (mock)";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in loginUser: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Login failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void AuthController::refreshToken(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        spdlog::info("POST /api/auth/refresh");
        
        // Parse request body
        auto json = req->getJsonObject();
        if (!json) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Invalid request body";
            errorJson["error"] = "JSON body required";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k400BadRequest);
            callback(response);
            return;
        }
        
        std::string refreshToken = (*json)["refreshToken"].asString();
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value dataJson;
        dataJson["accessToken"] = "new_mock_access_token";
        dataJson["refreshToken"] = "new_mock_refresh_token";
        dataJson["tokenType"] = "Bearer";
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Token refreshed successfully (mock)";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in refreshToken: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Token refresh failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void AuthController::logoutUser(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("POST /api/auth/logout");
        
        // Extract user from request (would use JWT validation in production)
        std::string userId = "mock_user_id";
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Logout successful (mock)";
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in logoutUser: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Logout failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void AuthController::extractUserFromRequest(const HttpRequestPtr& req, std::string& userId) {
    (void)req; // Suppress unused parameter warning
    // In production, this would extract and validate JWT token
    // For now, it's a placeholder
    userId = "mock_user_id";
}

} // namespace controllers
} // namespace presentation