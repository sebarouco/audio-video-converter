#include <drogon/drogon.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <random>

namespace fs = std::filesystem;

// Helper function to create JSON response
Json::Value createJsonResponse(bool success, const std::string& message) {
    Json::Value response;
    response["success"] = success;
    response["message"] = message;
    return response;
}

// Simple file upload handler
void handleFileUpload(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    if (req->method() != drogon::HttpMethod::Post) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, "Method not allowed"));
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    // Check if it's a multipart request
    auto contentType = req->getHeader("Content-Type");
    if (contentType.find("multipart/form-data") == std::string::npos) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, "Content-Type must be multipart/form-data"));
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    try {
        // For simplicity, just acknowledge the upload without processing
        // In a real implementation, you'd process the multipart data
        
        std::string fileId = "file_" + std::to_string(std::random_device{}());
        
        Json::Value response;
        response["success"] = true;
        response["message"] = "File uploaded successfully";
        response["data"]["fileId"] = fileId;
        response["data"]["fileName"] = "uploaded_file.bin";

        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);

    } catch (const std::exception &e) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, e.what()));
        resp->setStatusCode(drogon::k500InternalServerError);
        callback(resp);
    }
}

// Simple conversion start handler
void handleStartConversion(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    if (req->method() != drogon::HttpMethod::Post) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, "Method not allowed"));
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    try {
        auto bodyString = std::string(req->body());
        Json::Value body;
        Json::Reader reader;
        
        if (!reader.parse(bodyString, body)) {
            auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, "Invalid JSON"));
            resp->setStatusCode(drogon::k400BadRequest);
            callback(resp);
            return;
        }

        std::string fileId = body["fileId"].asString();
        std::string targetFormat = body["targetFormat"].asString();

        // Generate task ID
        std::string taskId = "task_" + std::to_string(std::random_device{}());

        // Simulate conversion (in real implementation, use FFmpeg)
        Json::Value response;
        response["success"] = true;
        response["message"] = "Conversion started";
        response["data"]["taskId"] = taskId;
        response["data"]["status"] = "processing";

        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);

    } catch (const std::exception &e) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(createJsonResponse(false, e.what()));
        resp->setStatusCode(drogon::k500InternalServerError);
        callback(resp);
    }
}

// Simple conversion progress handler
void handleGetProgress(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    std::string taskId = req->getParameter("taskId");

    // Simulate progress (in real implementation, check actual progress)
    Json::Value response;
    response["success"] = true;
    response["data"]["taskId"] = taskId;
    response["data"]["status"] = "completed";
    response["data"]["progress"] = 100;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

// Simple download handler
void handleDownload(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    std::string taskId = req->getParameter("taskId");

    // For demo, return a simple text file
    std::string content = "This is a converted file (demo)\nTask ID: " + taskId + "\n";
    
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(content);
    resp->setContentTypeString("text/plain");
    resp->addHeader("Content-Disposition", "attachment; filename=\"converted_demo.txt\"");
    callback(resp);
}

// Health check handler
void handleHealth(const drogon::HttpRequestPtr &req,
                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    Json::Value response;
    response["status"] = "healthy";
    response["message"] = "Backend is running";
    
    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

int main() {
    std::cout << "Starting Simple Online File Converter Backend..." << std::endl;

    // Set HTTP listener
    drogon::app().setLogPath("./")
           .setLogLevel(trantor::Logger::kWarn)
           .addListener("0.0.0.0", 8080)
           .setThreadNum(16)
           .enableRunAsDaemon()
           .setDocumentRoot("./")
           .run();

    // Register routes
    drogon::app().registerHandler("/api/health",
                                  &handleHealth,
                                  {drogon::Get});

    drogon::app().registerHandler("/api/files/upload",
                                  &handleFileUpload,
                                  {drogon::Post});

    drogon::app().registerHandler("/api/conversion/start",
                                  &handleStartConversion,
                                  {drogon::Post});

    drogon::app().registerHandler("/api/conversion/progress",
                                  &handleGetProgress,
                                  {drogon::Get});

    drogon::app().registerHandler("/api/conversion/{taskId}/download",
                                  &handleDownload,
                                  {drogon::Get});

    std::cout << "Server started on http://0.0.0.0:8080" << std::endl;
    std::cout << "API endpoints:" << std::endl;
    std::cout << "  GET  /api/health" << std::endl;
    std::cout << "  POST /api/files/upload" << std::endl;
    std::cout << "  POST /api/conversion/start" << std::endl;
    std::cout << "  GET  /api/conversion/progress?taskId={taskId}" << std::endl;
    std::cout << "  GET  /api/conversion/{taskId}/download" << std::endl;

    drogon::app().run();
    return 0;
}