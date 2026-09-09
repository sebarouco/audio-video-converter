#include "presentation/controllers/FileController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

using namespace drogon;

namespace presentation {
namespace controllers {

// Simple base64 decode implementation
static bool isBase64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::vector<uint8_t> base64Decode(const std::string& encoded) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uint8_t> result;
    
    int in_len = encoded.size();
    int i = 0;
    int in = 0;
    unsigned char char_array_4[4], char_array_3[3];
    
    while (in_len-- && (encoded[in] != '=') && isBase64(encoded[in])) {
        char_array_4[i++] = encoded[in];
        in++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; i < 3; i++)
                result.push_back(char_array_3[i]);
            i = 0;
        }
    }
    
    if (i) {
        for (int j = i; j < 4; j++)
            char_array_4[j] = 0;
        
        for (int j = 0; j < 4; j++)
            char_array_4[j] = chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (int j = 0; j < i - 1; j++)
            result.push_back(char_array_3[j]);
    }
    
    return result;
}

void FileController::uploadFile(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        spdlog::info("POST /api/files/upload");
        
        // Accept JSON body with file info and base64 data
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
        
        std::string fileName = (*json)["fileName"].asString();
        std::string fileData = (*json)["fileData"].asString(); // Base64 encoded
        
        if (fileName.empty() || fileData.empty()) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Missing required fields";
            errorJson["error"] = "fileName and fileData are required";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k400BadRequest);
            callback(response);
            return;
        }
        
        // Generate a unique file ID
        std::string fileId = "file_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Create upload directory
        std::string uploadDir = "./data/uploads";
        std::filesystem::create_directories(uploadDir);
        
        std::string filePath = uploadDir + "/" + fileId + "_" + fileName;
        
        // Decode base64 data
        std::vector<uint8_t> binaryData = base64Decode(fileData);
        
        // Save file to disk
        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to create file: " + filePath);
        }
        
        outFile.write(reinterpret_cast<const char*>(binaryData.data()), binaryData.size());
        outFile.close();
        
        spdlog::info("File saved successfully: {} (size: {} bytes)", filePath, binaryData.size());
        
        // Return success with file info
        Json::Value dataJson;
        dataJson["fileId"] = fileId;
        dataJson["fileName"] = fileName;
        dataJson["filePath"] = filePath;
        dataJson["fileSize"] = static_cast<Json::Int64>(binaryData.size());
        
        // Extract format from filename
        size_t dotPos = fileName.find_last_of(".");
        if (dotPos != std::string::npos) {
            dataJson["format"] = fileName.substr(dotPos + 1);
        } else {
            dataJson["format"] = "unknown";
        }
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "File uploaded successfully";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in uploadFile: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "File upload failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void FileController::getFileInfo(const HttpRequestPtr& req,
                                 std::function<void(const HttpResponsePtr&)>&& callback,
                                 const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("GET /api/files/{}", std::string_view(id));
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value dataJson;
        dataJson["id"] = id;
        dataJson["fileName"] = "mock_file.mp3";
        dataJson["filePath"] = "/mock/path/to/file";
        dataJson["fileSize"] = 1024000;
        dataJson["format"] = "mp3";
        dataJson["createdAt"] = "2024-01-01T00:00:00Z";
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "File info retrieved successfully (mock)";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in getFileInfo: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to get file info";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void FileController::deleteFile(const HttpRequestPtr& req,
                                std::function<void(const HttpResponsePtr&)>&& callback,
                                const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("DELETE /api/files/{}", id);
        
        // Execute use case (would need dependency injection in production)
        // For now, return a mock response
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "File deleted successfully (mock)";
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in deleteFile: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to delete file";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void FileController::validateFileUpload(const HttpRequestPtr& req) {
    // Simplified validation - would be expanded in production
    (void)req; // Suppress unused parameter warning
}

} // namespace controllers
} // namespace presentation