#pragma once

#include "application/interfaces/IUseCase.h"
#include "application/dto/ConversionTaskDTO.h"
#include "domain/repositories/IConversionTaskRepository.h"
#include "domain/services/IConversionService.h"
#include <memory>
#include <string>
#include <optional>

namespace application {
namespace usecases {
namespace conversion {

struct GetConversionProgressRequest {
    std::string taskId;
    std::string userId;
};

struct GetConversionProgressResponse {
    dto::ConversionProgressDTO progress;
    bool success;
    std::string message;
};

class GetConversionProgressUseCase : public interfaces::IUseCase<GetConversionProgressRequest, GetConversionProgressResponse> {
private:
    std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository_;
    std::shared_ptr<domain::services::IConversionService> conversionService_;
    
    bool validateTaskId(const std::string& taskId) const;
    bool validateUserId(const std::string& userId) const;
    bool checkUserAccess(const std::string& taskId, const std::string& userId) const;
    
public:
    GetConversionProgressUseCase(
        std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository,
        std::shared_ptr<domain::services::IConversionService> conversionService
    );
    
    GetConversionProgressResponse execute(const GetConversionProgressRequest& request) override;
    bool validateRequest(const GetConversionProgressRequest& request) const override;
    std::string getUseCaseName() const override { return "GetConversionProgressUseCase"; }
};

} // namespace conversion
} // namespace usecases
} // namespace application