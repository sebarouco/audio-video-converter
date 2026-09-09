#pragma once

#include "application/interfaces/IUseCase.h"
#include "application/dto/ConversionTaskDTO.h"
#include "domain/entities/ConversionTask.h"
#include "domain/repositories/IConversionTaskRepository.h"
#include "domain/services/IConversionService.h"
#include "domain/services/ITaskQueueService.h"
#include "domain/services/IFileStorageService.h"
#include "domain/valueobjects/FileFormat.h"
#include <memory>
#include <string>

namespace application {
namespace usecases {
namespace conversion {

struct StartConversionRequest {
    std::string fileId;
    std::string targetFormat;
    std::string userId;
    std::vector<std::string> options;
};

struct StartConversionResponse {
    dto::StartConversionResponse startConversionResponse;
    bool success;
    std::string message;
};

class StartConversionUseCase : public interfaces::IUseCase<StartConversionRequest, StartConversionResponse> {
private:
    std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository_;
    std::shared_ptr<domain::services::IConversionService> conversionService_;
    std::shared_ptr<domain::services::ITaskQueueService> taskQueueService_;
    std::shared_ptr<domain::services::IFileStorageService> fileStorageService_;
    
    bool validateFileId(const std::string& fileId) const;
    bool validateTargetFormat(const std::string& targetFormat) const;
    bool validateUserId(const std::string& userId) const;
    std::string generateTaskId() const;
    domain::valueobjects::FileFormat parseFileFormat(const std::string& format) const;
    
public:
    StartConversionUseCase(
        std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository,
        std::shared_ptr<domain::services::IConversionService> conversionService,
        std::shared_ptr<domain::services::ITaskQueueService> taskQueueService,
        std::shared_ptr<domain::services::IFileStorageService> fileStorageService
    );
    
    StartConversionResponse execute(const StartConversionRequest& request) override;
    bool validateRequest(const StartConversionRequest& request) const override;
    std::string getUseCaseName() const override { return "StartConversionUseCase"; }
};

} // namespace conversion
} // namespace usecases
} // namespace application