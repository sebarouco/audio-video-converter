#pragma once

#include "application/interfaces/IUseCase.h"
#include "application/dto/ConversionTaskDTO.h"
#include "domain/services/IFileStorageService.h"
#include "domain/valueobjects/FileFormat.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace application {
namespace usecases {
namespace conversion {

struct UploadFileRequest {
    std::string fileName;
    std::string contentType;
    size_t fileSize;
    std::vector<uint8_t> fileData;
    std::string userId;
};

struct UploadFileResponse {
    dto::UploadFileResponse uploadResponse;
    bool success;
    std::string message;
};

class UploadFileUseCase : public interfaces::IUseCase<UploadFileRequest, UploadFileResponse> {
private:
    std::shared_ptr<domain::services::IFileStorageService> fileStorageService_;
    
    bool validateFileName(const std::string& fileName) const;
    bool validateFileSize(size_t fileSize) const;
    bool validateFileFormat(const std::string& fileName) const;
    bool validateContentType(const std::string& contentType) const;
    std::string generateFileId() const;
    
public:
    explicit UploadFileUseCase(
        std::shared_ptr<domain::services::IFileStorageService> fileStorageService
    );
    
    UploadFileResponse execute(const UploadFileRequest& request) override;
    bool validateRequest(const UploadFileRequest& request) const override;
    std::string getUseCaseName() const override { return "UploadFileUseCase"; }
};

} // namespace conversion
} // namespace usecases
} // namespace application