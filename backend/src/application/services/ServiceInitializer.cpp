#include "application/services/ServiceInitializer.h"
#include "infrastructure/persistence/repositories/InMemoryConversionTaskRepository.h"
#include <spdlog/spdlog.h>

namespace application {
namespace services {

// Static dependency container instance
static application::services::DependencyContainer dependencyContainer;

application::services::DependencyContainer& ServiceInitializer::getDependencyContainer() {
    return dependencyContainer;
}

void ServiceInitializer::initializeServices(const std::string& configFile) {
    try {
        spdlog::info("Initializing application services...");
        
        // Load configuration
        infrastructure::config::AppConfig::load(configFile);
        auto& config = infrastructure::config::AppConfig::getInstance();
        
        // Initialize services in order
        initializeInfrastructureServices();
        initializeApplicationServices();
        initializePresentationServices();
        setupTaskQueueWorker();
        
        spdlog::info("All services initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize services: {}", e.what());
        throw;
    }
}

void ServiceInitializer::shutdownServices() {
    try {
        spdlog::info("Shutting down services...");
        dependencyContainer.clear();
        spdlog::info("Services shut down successfully");
    } catch (const std::exception& e) {
        spdlog::error("Error during service shutdown: {}", e.what());
    }
}

void ServiceInitializer::initializeInfrastructureServices() {
    auto& container = dependencyContainer;
    auto& config = infrastructure::config::AppConfig::getInstance();
    
    // Database connection - using SQLite with local file
    std::string dbPath = "data/converter.db";
    auto dbConnection = std::make_shared<infrastructure::external::DatabaseConnection>(dbPath);
    container.registerSingleton<infrastructure::external::DatabaseConnection>(dbConnection);
    
    // User repository
    auto userRepository = std::make_shared<infrastructure::persistence::repositories::SqlUserRepository>(dbConnection);
    container.registerSingleton<domain::repositories::IUserRepository>(userRepository);
    
    // Conversion task repository (in-memory implementation)
    auto conversionTaskRepository = std::make_shared<infrastructure::persistence::repositories::InMemoryConversionTaskRepository>();
    container.registerSingleton<domain::repositories::IConversionTaskRepository>(conversionTaskRepository);
    
    // File storage service
    auto fileStorageService = std::make_shared<infrastructure::services::storage::LocalFileStorageService>(
        config.getUploadDir(),
        config.getConvertedDir(),
        config.getTempDir(),
        config.getMaxFileSize()
    );
    container.registerSingleton<domain::services::IFileStorageService>(fileStorageService);
    
    // Auth service
    auto authService = std::make_shared<infrastructure::services::auth::JwtAuthService>(
        config.getJwtSecret(),
        config.getJwtAlgorithm(),
        config.getJwtExpirationHours(),
        config.getJwtRefreshExpirationDays()
    );
    container.registerSingleton<domain::services::IAuthService>(authService);
    
    // Conversion service
    auto conversionService = std::make_shared<infrastructure::services::conversion::FFmpegConverter>(
        config.getFFmpegPath(),
        config.getFFmpegThreads(),
        config.getFFmpegTimeoutSeconds()
    );
    container.registerSingleton<domain::services::IConversionService>(conversionService);
    
    // Task queue service
    auto taskQueueService = std::make_shared<infrastructure::services::queue::RedisTaskQueueService>(
        config.getRedisHost(),
        config.getRedisPort(),
        config.getRedisPassword(),
        config.getRedisDb()
    );
    container.registerSingleton<domain::services::ITaskQueueService>(taskQueueService);
    
    spdlog::info("Infrastructure services initialized");
}

void ServiceInitializer::initializeApplicationServices() {
    auto& container = dependencyContainer;
    
    // Auth use cases
    auto registerUserUseCase = std::make_shared<application::usecases::auth::RegisterUserUseCase>(
        container.resolve<domain::repositories::IUserRepository>(),
        container.resolve<domain::services::IAuthService>()
    );
    container.registerSingleton<application::usecases::auth::RegisterUserUseCase>(registerUserUseCase);
    
    auto loginUserUseCase = std::make_shared<application::usecases::auth::LoginUserUseCase>(
        container.resolve<domain::repositories::IUserRepository>(),
        container.resolve<domain::services::IAuthService>()
    );
    container.registerSingleton<application::usecases::auth::LoginUserUseCase>(loginUserUseCase);
    
    // Conversion use cases
    auto uploadFileUseCase = std::make_shared<application::usecases::conversion::UploadFileUseCase>(
        container.resolve<domain::services::IFileStorageService>()
    );
    container.registerSingleton<application::usecases::conversion::UploadFileUseCase>(uploadFileUseCase);
    
    auto startConversionUseCase = std::make_shared<application::usecases::conversion::StartConversionUseCase>(
        container.resolve<domain::repositories::IConversionTaskRepository>(),
        container.resolve<domain::services::IConversionService>(),
        container.resolve<domain::services::ITaskQueueService>(),
        container.resolve<domain::services::IFileStorageService>()
    );
    container.registerSingleton<application::usecases::conversion::StartConversionUseCase>(startConversionUseCase);
    
    auto getConversionProgressUseCase = std::make_shared<application::usecases::conversion::GetConversionProgressUseCase>(
        container.resolve<domain::repositories::IConversionTaskRepository>(), // This would need to be implemented
        container.resolve<domain::services::IConversionService>()
    );
    container.registerSingleton<application::usecases::conversion::GetConversionProgressUseCase>(getConversionProgressUseCase);
    
    spdlog::info("Application services initialized");
}

void ServiceInitializer::initializePresentationServices() {
    // Controllers would be initialized here when we implement them properly
    spdlog::info("Presentation services initialized");
}

void ServiceInitializer::setupTaskQueueWorker() {
    auto& container = dependencyContainer;
    auto taskQueueService = container.resolve<domain::services::ITaskQueueService>();
    auto conversionService = container.resolve<domain::services::IConversionService>();
    auto fileStorageService = container.resolve<domain::services::IFileStorageService>();
    auto conversionTaskRepository = container.resolve<domain::repositories::IConversionTaskRepository>();
    
    // Create task processor function
    auto taskProcessor = [conversionService, fileStorageService, conversionTaskRepository](const domain::services::Task& task) {
        try {
            std::string taskId = task.id;
            std::string taskType = task.type;
            spdlog::info("Processing task: {} of type: {}", taskId, taskType);
            
            if (task.type == "conversion") {
                // Parse payload (format: "fileId:targetFormat")
                size_t colonPos = task.payload.find(':');
                if (colonPos == std::string::npos) {
                    spdlog::error("Invalid task payload format: {}", task.payload);
                    return;
                }
                
                std::string fileId = task.payload.substr(0, colonPos);
                std::string targetFormat = task.payload.substr(colonPos + 1);
                
                // Get the conversion task from repository
                auto conversionTask = conversionTaskRepository->findById(task.id);
                if (!conversionTask) {
                    spdlog::error("Conversion task not found: {}", task.id);
                    return;
                }
                
                // Build conversion request
                domain::services::ConversionRequest request;
                request.taskId = task.id;
                request.inputFilePath = conversionTask->getInputFilePath();
                request.outputFilePath = conversionTask->getOutputFilePath();
                request.sourceFormat = conversionTask->getSourceFormat();
                request.targetFormat = conversionTask->getTargetFormat();
                request.additionalOptions = conversionService->getConversionOptions(
                    request.sourceFormat, request.targetFormat);
                
                // Execute conversion
                auto result = conversionService->convert(request);
                
                if (result.success) {
                    conversionTask->markAsCompleted();
                    std::string taskIdStr = task.id;
                    spdlog::info("Conversion task completed successfully: {}", taskIdStr);
                } else {
                    conversionTask->markAsFailed(result.errorMessage);
                    std::string taskIdStr = task.id;
                    std::string errorMsg = result.errorMessage;
                    spdlog::error("Conversion task failed: {} - {}", taskIdStr, errorMsg);
                }
                
                conversionTaskRepository->update(*conversionTask);
            }
            
        } catch (const std::exception& e) {
            spdlog::error("Error processing task {}: {}", task.id, e.what());
        }
    };
    
    // Start the worker
    auto redisQueueService = std::dynamic_pointer_cast<infrastructure::services::queue::RedisTaskQueueService>(taskQueueService);
    if (redisQueueService) {
        redisQueueService->startWorker(taskProcessor);
        spdlog::info("Task queue worker started successfully");
    } else {
        spdlog::warn("Failed to start task queue worker - service is not RedisTaskQueueService");
    }
}

} // namespace services
} // namespace application