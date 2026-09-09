#include "application/usecases/conversion/GetConversionProgressUseCase.h"
#include <spdlog/spdlog.h>

namespace application {
namespace usecases {
namespace conversion {

GetConversionProgressUseCase::GetConversionProgressUseCase(
    std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository,
    std::shared_ptr<domain::services::IConversionService> conversionService
) : conversionTaskRepository_(conversionTaskRepository),
    conversionService_(conversionService) {
    
    if (!conversionTaskRepository_ || !conversionService_) {
        throw std::invalid_argument("Conversion repository and service cannot be null");
    }
}

bool GetConversionProgressUseCase::validateTaskId(const std::string& taskId) const {
    return !taskId.empty() && taskId.length() == 36; // UUID format
}

bool GetConversionProgressUseCase::validateUserId(const std::string& userId) const {
    return !userId.empty() && userId.length() == 36; // UUID format
}

bool GetConversionProgressUseCase::checkUserAccess(const std::string& taskId, const std::string& userId) const {
    auto task = conversionTaskRepository_->findById(taskId);
    if (!task.has_value()) {
        return false;
    }
    
    return task->getUserId() == userId;
}

bool GetConversionProgressUseCase::validateRequest(const GetConversionProgressRequest& request) const {
    if (!validateTaskId(request.taskId)) {
        spdlog::warn("Invalid task ID: {}", request.taskId);
        return false;
    }
    
    if (!validateUserId(request.userId)) {
        spdlog::warn("Invalid user ID: {}", request.userId);
        return false;
    }
    
    if (!checkUserAccess(request.taskId, request.userId)) {
        spdlog::warn("User {} does not have access to task {}", request.userId, request.taskId);
        return false;
    }
    
    return true;
}

GetConversionProgressResponse GetConversionProgressUseCase::execute(const GetConversionProgressRequest& request) {
    try {
        spdlog::info("Executing GetConversionProgressUseCase for task: {} by user: {}", request.taskId, request.userId);
        
        // Get task from repository
        auto task = conversionTaskRepository_->findById(request.taskId);
        if (!task.has_value()) {
            spdlog::warn("Task not found: {}", request.taskId);
            return {
                .progress = {},
                .success = false,
                .message = "Conversion task not found"
            };
        }
        
        // Get progress from conversion service
        auto progress = conversionService_->getProgress(request.taskId);
        
        // Build response
        dto::ConversionProgressDTO progressDTO;
        progressDTO.taskId = progress.taskId;
        progressDTO.progress = progress.progress;
        progressDTO.currentOperation = progress.currentOperation;
        progressDTO.speed = progress.speed;
        progressDTO.eta = progress.eta;
        progressDTO.status = domain::valueobjects::ConversionStatusHelper::toString(progress.status);
        
        spdlog::info("Conversion progress retrieved successfully for task: {}", request.taskId);
        
        return {
            .progress = progressDTO,
            .success = true,
            .message = "Progress retrieved successfully"
        };
        
    } catch (const std::exception& e) {
        spdlog::error("Error in GetConversionProgressUseCase: {}", e.what());
        return {
            .progress = {},
            .success = false,
            .message = std::string("Failed to get progress: ") + e.what()
        };
    }
}

} // namespace conversion
} // namespace usecases
} // namespace application