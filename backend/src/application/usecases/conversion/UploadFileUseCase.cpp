#include "application/usecases/conversion/UploadFileUseCase.h"
#include <random>
#include <spdlog/spdlog.h>
#include <sstream>

namespace application {
namespace usecases {
namespace conversion {

UploadFileUseCase::UploadFileUseCase(
    std::shared_ptr<domain::services::IFileStorageService> fileStorageService
) : fileStorageService_(fileStorageService) {
    if (!fileStorageService_) {
        throw std::invalid_argument("File storage service cannot be null");
    }
}

bool UploadFileUseCase::validateFileName(const std::string& fileName) const {
    if (fileName.empty() || fileName.length() > 255) {
        return false;
    }
    
    // Check for invalid characters
    const std::string invalidChars = "<>:\"/\\|?*";
    for (char c : invalidChars) {
        if (fileName.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool UploadFileUseCase::validateFileSize(size_t fileSize) const {
    // Max file size: 512MB (536870912 bytes)
    const size_t MAX_FILE_SIZE = 536870912;
    return fileSize > 0 && fileSize <= MAX_FILE_SIZE;
}

bool UploadFileUseCase::validateFileFormat(const std::string& fileName) const {
    // Extract file extension
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == std::string::npos) {
        return false;
    }
    
    std::string extension = fileName.substr(dotPos + 1);
    
    // Convert to lowercase
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Supported formats
    const std::vector<std::string> supportedFormats = {
        "mp3", "wav", "flac", "ogg", "aac", "m4a", "wma", "opus", "alac",
        "mp4", "avi", "mov", "mkv", "webm", "flv", "wmv", "m4v", "mpeg"
    };
    
    return std::find(supportedFormats.begin(), supportedFormats.end(), extension) != supportedFormats.end();
}

bool UploadFileUseCase::validateContentType(const std::string& contentType) const {
    // Allow common content types
    const std::vector<std::string> validContentTypes = {
        "audio/mpeg", "audio/wav", "audio/flac", "audio/ogg", "audio/aac", "audio/m4a",
        "video/mp4", "video/avi", "video/quicktime", "video/x-matroska", "video/webm"
    };
    
    return std::find(validContentTypes.begin(), validContentTypes.end(), contentType) != validContentTypes.end();
}

std::string UploadFileUseCase::generateFileId() const {
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

bool UploadFileUseCase::validateRequest(const UploadFileRequest& request) const {
    if (!validateFileName(request.fileName)) {
        spdlog::warn("Invalid file name: {}", request.fileName);
        return false;
    }
    
    if (!validateFileSize(request.fileSize)) {
        spdlog::warn("Invalid file size: {}", request.fileSize);
        return false;
    }
    
    if (!validateFileFormat(request.fileName)) {
        spdlog::warn("Unsupported file format: {}", request.fileName);
        return false;
    }
    
    if (!validateContentType(request.contentType)) {
        spdlog::warn("Invalid content type: {}", request.contentType);
        return false;
    }
    
    if (request.fileData.empty()) {
        spdlog::warn("Empty file data");
        return false;
    }
    
    if (request.userId.empty()) {
        spdlog::warn("Empty user ID");
        return false;
    }
    
    return true;
}

UploadFileResponse UploadFileUseCase::execute(const UploadFileRequest& request) {
    try {
        spdlog::info("Executing UploadFileUseCase for file: {} by user: {}", request.fileName, request.userId);
        
        // Store file
        std::string filePath = fileStorageService_->storeFile(request.fileName, request.fileData);
        
        if (filePath.empty()) {
            spdlog::error("Failed to store file: {}", request.fileName);
            return {
                .uploadResponse = {},
                .success = false,
                .message = "Failed to store file"
            };
        }
        
        // Extract file format
        size_t dotPos = request.fileName.find_last_of('.');
        std::string format = (dotPos != std::string::npos) ? request.fileName.substr(dotPos + 1) : "";
        std::transform(format.begin(), format.end(), format.begin(), ::tolower);
        
        // Generate file ID
        std::string fileId = generateFileId();
        
        // Build response
        dto::UploadFileResponse uploadResponse;
        uploadResponse.fileId = fileId;
        uploadResponse.fileName = request.fileName;
        uploadResponse.filePath = filePath;
        uploadResponse.fileSize = request.fileSize;
        uploadResponse.format = format;
        
        spdlog::info("File uploaded successfully: {} with ID: {}", request.fileName, fileId);
        
        return {
            .uploadResponse = uploadResponse,
            .success = true,
            .message = "File uploaded successfully"
        };
        
    } catch (const std::exception& e) {
        spdlog::error("Error in UploadFileUseCase: {}", e.what());
        return {
            .uploadResponse = {},
            .success = false,
            .message = std::string("File upload failed: ") + e.what()
        };
    }
}

} // namespace conversion
} // namespace usecases
} // namespace application