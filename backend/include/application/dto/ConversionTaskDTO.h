#pragma once

#include <string>
#include <chrono>
#include "domain/valueobjects/FileFormat.h"
#include "domain/valueobjects/ConversionStatus.h"

namespace application {
namespace dto {

struct ConversionTaskDTO {
    std::string id;
    std::string userId;
    std::string originalFileName;
    std::string originalFilePath;
    std::string convertedFilePath;
    std::string sourceFormat;
    std::string targetFormat;
    std::string status;
    int progress;
    std::string errorMessage;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point startedAt;
    std::chrono::system_clock::time_point completedAt;
    
    ConversionTaskDTO() = default;
};

struct UploadFileRequest {
    std::string fileName;
    std::string contentType;
    size_t fileSize;
    std::vector<uint8_t> fileData;
};

struct UploadFileResponse {
    std::string fileId;
    std::string fileName;
    std::string filePath;
    size_t fileSize;
    std::string format;
};

struct StartConversionRequest {
    std::string fileId;
    std::string targetFormat;
    std::vector<std::string> options;
};

struct StartConversionResponse {
    std::string taskId;
    std::string status;
    std::string message;
};

struct ConversionProgressDTO {
    std::string taskId;
    int progress;
    std::string currentOperation;
    double speed;
    std::string eta;
    std::string status;
};

struct ConversionResultDTO {
    std::string taskId;
    bool success;
    std::string outputFilePath;
    std::string errorMessage;
    size_t outputFileSize;
    std::chrono::system_clock::time_point completedAt;
};

} // namespace dto
} // namespace application