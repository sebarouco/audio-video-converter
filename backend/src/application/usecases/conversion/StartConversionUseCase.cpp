#include "application/usecases/conversion/StartConversionUseCase.h"
#include <random>
#include <spdlog/spdlog.h>
#include <sstream>

namespace application {
namespace usecases {
namespace conversion {

StartConversionUseCase::StartConversionUseCase(
    std::shared_ptr<domain::repositories::IConversionTaskRepository> conversionTaskRepository,
    std::shared_ptr<domain::services::IConversionService> conversionService,
    std::shared_ptr<domain::services::ITaskQueueService> taskQueueService,
    std::shared_ptr<domain::services::IFileStorageService> fileStorageService
) : conversionTaskRepository_(conversionTaskRepository),
    conversionService_(conversionService),
    taskQueueService_(taskQueueService),
    fileStorageService_(fileStorageService) {
    
    if (!conversionTaskRepository_ || !conversionService_ || !taskQueueService_ || !fileStorageService_) {
        throw std::invalid_argument("Conversion repository, service, task queue, and file storage cannot be null");
    }
}

bool StartConversionUseCase::validateFileId(const std::string& fileId) const {
    return !fileId.empty() && fileId.length() == 36; // UUID format
}

bool StartConversionUseCase::validateTargetFormat(const std::string& targetFormat) const {
    domain::valueobjects::FileFormat format(targetFormat);
    return format.isSupported();
}

bool StartConversionUseCase::validateUserId(const std::string& userId) const {
    return !userId.empty() && userId.length() == 36; // UUID format
}

std::string StartConversionUseCase::generateTaskId() const {
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

domain::valueobjects::FileFormat StartConversionUseCase::parseFileFormat(const std::string& format) const {
    return domain::valueobjects::FileFormat(format);
}

bool StartConversionUseCase::validateRequest(const StartConversionRequest& request) const {
    if (!validateFileId(request.fileId)) {
        spdlog::warn("Invalid file ID: {}", request.fileId);
        return false;
    }
    
    if (!validateTargetFormat(request.targetFormat)) {
        spdlog::warn("Invalid target format: {}", request.targetFormat);
        return false;
    }
    
    if (!validateUserId(request.userId)) {
        spdlog::warn("Invalid user ID: {}", request.userId);
        return false;
    }
    
    return true;
}

StartConversionResponse StartConversionUseCase::execute(const StartConversionRequest& request) {
    try {
        spdlog::info("Executing StartConversionUseCase for file: {} by user: {}", request.fileId, request.userId);
        
        // Generate task ID
        std::string taskId = generateTaskId();
        
        // Parse formats
        domain::valueobjects::FileFormat targetFormat = parseFileFormat(request.targetFormat);
        
        // For now, we'll assume the fileId is actually the file path
        // In a real implementation, you'd look up the file in storage
        std::string inputFilePath = request.fileId;
        
        // Generate output file path
        std::string outputFilePath = inputFilePath + ".converted." + targetFormat.getExtension();
        
        // Detect source format from input file extension
        size_t dotPos = inputFilePath.find_last_of('.');
        std::string sourceExt = (dotPos != std::string::npos) ? inputFilePath.substr(dotPos + 1) : "mp3";
        domain::valueobjects::FileFormat sourceFormat = parseFileFormat(sourceExt);
        
        // Create conversion task
        domain::entities::ConversionTask task(
            taskId,
            request.userId,
            inputFilePath,
            sourceFormat,
            targetFormat
        );
        task.setOutputFilePath(outputFilePath);
        
        // Save task to repository
        conversionTaskRepository_->save(task);
        
        // Create task for queue
        domain::services::Task queueTask;
        queueTask.id = taskId;
        queueTask.type = "conversion";
        queueTask.payload = inputFilePath + ":" + targetFormat.getExtension();
        queueTask.priority = domain::services::TaskPriority::NORMAL;
        queueTask.createdAt = std::chrono::system_clock::now();
        queueTask.retryCount = 0;
        queueTask.maxRetries = 3;
        
        // Enqueue task
        std::string queueTaskId = taskQueueService_->enqueueTask(queueTask);
        
        if (queueTaskId.empty()) {
            spdlog::error("Failed to enqueue conversion task: {}", taskId);
            task.markAsFailed("Failed to enqueue task");
            conversionTaskRepository_->update(task);
            
            return {
                .startConversionResponse = {},
                .success = false,
                .message = "Failed to enqueue conversion task"
            };
        }
        
        // Update task status to queued
        task.markAsStarted(); // This will set status to PROCESSING initially
        conversionTaskRepository_->update(task);
        
        // Build response
        dto::StartConversionResponse startConversionResponse;
        startConversionResponse.taskId = taskId;
        startConversionResponse.status = "QUEUED";
        startConversionResponse.message = "Conversion task queued successfully";
        
        spdlog::info("Conversion task started successfully: {}", taskId);
        
        return {
            .startConversionResponse = startConversionResponse,
            .success = true,
            .message = "Conversion started successfully"
        };
        
    } catch (const std::exception& e) {
        spdlog::error("Error in StartConversionUseCase: {}", e.what());
        return {
            .startConversionResponse = {},
            .success = false,
            .message = std::string("Failed to start conversion: ") + e.what()
        };
    }
}

} // namespace conversion
} // namespace usecases
} // namespace application