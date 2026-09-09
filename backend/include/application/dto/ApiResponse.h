#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace application {
namespace dto {

template<typename T>
struct ApiResponse {
    bool success;
    std::string message;
    std::optional<T> data;
    std::optional<std::string> error;
    std::chrono::system_clock::time_point timestamp;
    
    ApiResponse() : success(false), timestamp(std::chrono::system_clock::now()) {}
    
    static ApiResponse<T> successResponse(const std::string& message, const T& data) {
        ApiResponse<T> response;
        response.success = true;
        response.message = message;
        response.data = data;
        response.timestamp = std::chrono::system_clock::now();
        return response;
    }
    
    static ApiResponse<T> errorResponse(const std::string& message, const std::string& error) {
        ApiResponse<T> response;
        response.success = false;
        response.message = message;
        response.error = error;
        response.timestamp = std::chrono::system_clock::now();
        return response;
    }
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["success"] = success;
        j["message"] = message;
        j["timestamp"] = std::chrono::system_clock::to_time_t(timestamp);
        
        if (data.has_value()) {
            j["data"] = data.value();
        }
        
        if (error.has_value()) {
            j["error"] = error.value();
        }
        
        return j;
    }
};

struct EmptyResponse {};

} // namespace dto
} // namespace application