#pragma once

#include "application/services/DependencyContainer.h"
#include "infrastructure/config/AppConfig.h"
#include "infrastructure/external/DatabaseConnection.h"
#include "infrastructure/persistence/repositories/SqlUserRepository.h"
#include "infrastructure/persistence/repositories/InMemoryConversionTaskRepository.h"
#include "infrastructure/services/storage/LocalFileStorageService.h"
#include "infrastructure/services/auth/JwtAuthService.h"
#include "infrastructure/services/conversion/FFmpegConverter.h"
#include "infrastructure/services/queue/RedisTaskQueueService.h"
#include "application/usecases/auth/RegisterUserUseCase.h"
#include "application/usecases/auth/LoginUserUseCase.h"
#include "application/usecases/conversion/UploadFileUseCase.h"
#include "application/usecases/conversion/StartConversionUseCase.h"
#include "application/usecases/conversion/GetConversionProgressUseCase.h"
#include <memory>

namespace application {
namespace services {

class ServiceInitializer {
public:
    static void initializeServices(const std::string& configFile = "config/config.json");
    static void shutdownServices();
    static application::services::DependencyContainer& getDependencyContainer();
    
private:
    static void initializeInfrastructureServices();
    static void initializeApplicationServices();
    static void initializePresentationServices();
    static void setupTaskQueueWorker();
};

} // namespace services
} // namespace application