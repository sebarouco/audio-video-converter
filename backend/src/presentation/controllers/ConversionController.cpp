#include "presentation/controllers/ConversionController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "application/services/ServiceInitializer.h"
#include "domain/valueobjects/FileFormat.h"

using namespace drogon;

namespace presentation {
namespace controllers {

std::shared_ptr<application::usecases::conversion::StartConversionUseCase> ConversionController::getStartConversionUseCase() {
    // Simplified - return nullptr for now, will be implemented with proper DI
    return nullptr;
}

std::shared_ptr<application::usecases::conversion::GetConversionProgressUseCase> ConversionController::getGetProgressUseCase() {
    // Simplified - return nullptr for now, will be implemented with proper DI
    return nullptr;
}

std::shared_ptr<domain::services::IConversionService> ConversionController::getConversionService() {
    try {
        auto& container = application::services::ServiceInitializer::getDependencyContainer();
        return container.resolve<domain::services::IConversionService>();
    } catch (const std::exception& e) {
        spdlog::error("Failed to get conversion service: {}", e.what());
        return nullptr;
    }
}

void ConversionController::startConversion(const HttpRequestPtr& req,
                                          std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        spdlog::info("POST /api/conversion/start");
        
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
        
        std::string inputFilePath = (*json)["inputFilePath"].asString();
        std::string targetFormat = (*json)["targetFormat"].asString();
        
        // Get conversion service
        auto conversionService = getConversionService();
        if (!conversionService) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Conversion service not available";
            errorJson["error"] = "Service initialization failed";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k500InternalServerError);
            callback(response);
            return;
        }
        
        // Generate task ID
        std::string taskId = "task_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Create output file path
        std::filesystem::path inputPath(inputFilePath);
        std::string outputDir = "./data/converted";
        std::filesystem::create_directories(outputDir);
        std::string outputFilePath = outputDir + "/" + taskId + "." + targetFormat;
        
        // Determine source format from input file extension
        std::string sourceFormat = inputPath.extension().string().substr(1); // Remove the dot
        
        // Build conversion request
        domain::services::ConversionRequest request;
        request.taskId = taskId;
        request.inputFilePath = inputFilePath;
        request.outputFilePath = outputFilePath;
        request.sourceFormat = domain::valueobjects::FileFormat::fromExtension(sourceFormat);
        request.targetFormat = domain::valueobjects::FileFormat::fromExtension(targetFormat);
        
        // Store the output path for later download
        {
            std::lock_guard<std::mutex> lock(tasksMutex_);
            conversionTasks_[taskId] = outputFilePath;
        }
        
        // Start conversion (async)
        auto result = conversionService->convert(request);
        
        Json::Value dataJson;
        dataJson["taskId"] = taskId;
        dataJson["status"] = result.success ? "PROCESSING" : "FAILED";
        dataJson["message"] = result.success ? "Conversion started" : "Conversion failed: " + result.errorMessage;
        
        Json::Value responseJson;
        responseJson["success"] = result.success;
        responseJson["message"] = result.success ? "Conversion started successfully" : "Conversion failed";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in startConversion: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to start conversion";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::getConversionStatus(const HttpRequestPtr& req,
                                                std::function<void(const HttpResponsePtr&)>&& callback,
                                                const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("GET /api/conversion/{}", std::string_view(id));
        
        // Get conversion service
        auto conversionService = getConversionService();
        if (!conversionService) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Conversion service not available";
            errorJson["error"] = "Service initialization failed";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k500InternalServerError);
            callback(response);
            return;
        }
        
        // Get progress to determine status
        auto progress = conversionService->getProgress(id);
        
        Json::Value dataJson;
        dataJson["taskId"] = id;
        dataJson["progress"] = progress.progress;
        
        // Convert status enum to string
        std::string statusStr;
        switch (progress.status) {
            case domain::valueobjects::ConversionStatus::PENDING:
                statusStr = "PENDING";
                break;
            case domain::valueobjects::ConversionStatus::PROCESSING:
                statusStr = "PROCESSING";
                break;
            case domain::valueobjects::ConversionStatus::COMPLETED:
                statusStr = "COMPLETED";
                break;
            case domain::valueobjects::ConversionStatus::FAILED:
                statusStr = "FAILED";
                break;
            case domain::valueobjects::ConversionStatus::CANCELLED:
                statusStr = "CANCELLED";
                break;
            default:
                statusStr = "UNKNOWN";
        }
        dataJson["status"] = statusStr;
        dataJson["message"] = progress.currentOperation;
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Conversion status retrieved successfully";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in getConversionStatus: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to get conversion status";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::getConversionProgress(const HttpRequestPtr& req,
                                                std::function<void(const HttpResponsePtr&)>&& callback,
                                                const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("GET /api/conversion/{}/progress", std::string_view(id));
        
        // Get conversion service
        auto conversionService = getConversionService();
        if (!conversionService) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Conversion service not available";
            errorJson["error"] = "Service initialization failed";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k500InternalServerError);
            callback(response);
            return;
        }
        
        // Get real progress from conversion service
        auto progress = conversionService->getProgress(id);
        
        Json::Value dataJson;
        dataJson["taskId"] = progress.taskId;
        dataJson["progress"] = progress.progress;
        dataJson["currentOperation"] = progress.currentOperation;
        dataJson["speed"] = progress.speed;
        dataJson["eta"] = progress.eta;
        
        // Convert status enum to string
        std::string statusStr;
        switch (progress.status) {
            case domain::valueobjects::ConversionStatus::PENDING:
                statusStr = "PENDING";
                break;
            case domain::valueobjects::ConversionStatus::PROCESSING:
                statusStr = "PROCESSING";
                break;
            case domain::valueobjects::ConversionStatus::COMPLETED:
                statusStr = "COMPLETED";
                break;
            case domain::valueobjects::ConversionStatus::FAILED:
                statusStr = "FAILED";
                break;
            case domain::valueobjects::ConversionStatus::CANCELLED:
                statusStr = "CANCELLED";
                break;
            default:
                statusStr = "UNKNOWN";
        }
        dataJson["status"] = statusStr;
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Conversion progress retrieved successfully";
        responseJson["data"] = dataJson;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in getConversionProgress: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to get conversion progress";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::downloadConvertedFile(const HttpRequestPtr& req,
                                                 std::function<void(const HttpResponsePtr&)>&& callback,
                                                 const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("GET /api/conversion/{}/download", std::string_view(id));
        
        // Get the output file path from our task storage
        std::string outputFilePath;
        {
            std::lock_guard<std::mutex> lock(tasksMutex_);
            auto it = conversionTasks_.find(id);
            if (it == conversionTasks_.end()) {
                Json::Value errorJson;
                errorJson["success"] = false;
                errorJson["message"] = "Conversion task not found";
                errorJson["error"] = "Task ID: " + id;
                
                auto response = HttpResponse::newHttpJsonResponse(errorJson);
                response->setStatusCode(k404NotFound);
                callback(response);
                return;
            }
            outputFilePath = it->second;
        }
        
        // Check if the file exists
        if (!std::filesystem::exists(outputFilePath)) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Converted file not found";
            errorJson["error"] = "File path: " + outputFilePath;
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k404NotFound);
            callback(response);
            return;
        }
        
        // Determine content type based on file extension
        std::filesystem::path filePath(outputFilePath);
        std::string extension = filePath.extension().string();
        std::string contentType;
        
        if (extension == ".mp3") {
            contentType = "audio/mpeg";
        } else if (extension == ".wav") {
            contentType = "audio/wav";
        } else if (extension == ".flac") {
            contentType = "audio/flac";
        } else if (extension == ".ogg") {
            contentType = "audio/ogg";
        } else if (extension == ".mp4") {
            contentType = "video/mp4";
        } else if (extension == ".avi") {
            contentType = "video/x-msvideo";
        } else if (extension == ".mov") {
            contentType = "video/quicktime";
        } else if (extension == ".webm") {
            contentType = "video/webm";
        } else if (extension == ".mkv") {
            contentType = "video/x-matroska";
        } else {
            contentType = "application/octet-stream";
        }
        
        // Read the file
        std::ifstream file(outputFilePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open converted file: " + outputFilePath);
        }
        
        std::string fileContent((std::istreambuf_iterator<char>(file)), 
                               std::istreambuf_iterator<char>());
        file.close();
        
        // Create response with file content
        auto response = HttpResponse::newHttpResponse();
        response->setBody(fileContent);
        response->setContentTypeString(contentType);
        response->addHeader("Content-Disposition", "attachment; filename=\"converted_" + id + extension + "\"");
        response->setStatusCode(k200OK);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in downloadConvertedFile: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to download file";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::cancelConversion(const HttpRequestPtr& req,
                                           std::function<void(const HttpResponsePtr&)>&& callback,
                                           const std::string& id) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("DELETE /api/conversion/{}", std::string_view(id));
        
        // Get conversion service
        auto conversionService = getConversionService();
        if (!conversionService) {
            Json::Value errorJson;
            errorJson["success"] = false;
            errorJson["message"] = "Conversion service not available";
            errorJson["error"] = "Service initialization failed";
            
            auto response = HttpResponse::newHttpJsonResponse(errorJson);
            response->setStatusCode(k500InternalServerError);
            callback(response);
            return;
        }
        
        // Cancel the conversion
        conversionService->cancelConversion(id);
        
        // Remove from our task storage
        {
            std::lock_guard<std::mutex> lock(tasksMutex_);
            conversionTasks_.erase(id);
        }
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Conversion cancelled successfully";
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in cancelConversion: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to cancel conversion";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::getSupportedFormats(const HttpRequestPtr& req,
                                              std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("GET /api/formats");
        
        // Supported formats
        Json::Value formatsArray(Json::arrayValue);
        formatsArray.append("mp3");
        formatsArray.append("wav");
        formatsArray.append("flac");
        formatsArray.append("ogg");
        formatsArray.append("m4a");
        formatsArray.append("mp4");
        formatsArray.append("avi");
        formatsArray.append("mov");
        formatsArray.append("webm");
        formatsArray.append("mkv");
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "Supported formats retrieved successfully";
        responseJson["data"] = formatsArray;
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in getSupportedFormats: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "Failed to get supported formats";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

void ConversionController::convertFile(const HttpRequestPtr& req,
                                      std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        (void)req; // Suppress unused parameter warning
        spdlog::info("POST /api/convert");
        
        // For now, return a mock response since we can't handle multipart in this Drogon version
        // In production, this would:
        // 1. Accept the uploaded file
        // 2. Extract format from request
        // 3. Use FFmpeg to convert the file
        // 4. Return the converted file
        
        Json::Value responseJson;
        responseJson["success"] = true;
        responseJson["message"] = "File conversion endpoint ready - multipart handling needed";
        responseJson["data"]["note"] = "This endpoint needs proper multipart file handling in Drogon";
        
        auto response = HttpResponse::newHttpJsonResponse(responseJson);
        callback(response);
        
    } catch (const std::exception& e) {
        spdlog::error("Error in convertFile: {}", e.what());
        Json::Value errorJson;
        errorJson["success"] = false;
        errorJson["message"] = "File conversion failed";
        errorJson["error"] = e.what();
        
        auto response = HttpResponse::newHttpJsonResponse(errorJson);
        response->setStatusCode(k500InternalServerError);
        callback(response);
    }
}

std::string ConversionController::extractUserIdFromRequest(const HttpRequestPtr& req) {
    // In production, this would extract and validate JWT token
    // For now, it's a placeholder
    (void)req; // Suppress unused parameter warning
    return "mock_user_id";
}

} // namespace controllers
} // namespace presentation