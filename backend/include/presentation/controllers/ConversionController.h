#pragma once

#include <drogon/HttpController.h>
#include "application/usecases/conversion/StartConversionUseCase.h"
#include "application/usecases/conversion/GetConversionProgressUseCase.h"
#include "application/dto/ConversionTaskDTO.h"
#include "application/services/DependencyContainer.h"
#include "domain/services/IConversionService.h"
#include <map>
#include <mutex>

using namespace drogon;

namespace presentation {
namespace controllers {

class ConversionController : public drogon::HttpController<ConversionController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ConversionController::startConversion, "/api/conversion/start", Post);
    ADD_METHOD_TO(ConversionController::getConversionStatus, "/api/conversion/{id}", Get);
    ADD_METHOD_TO(ConversionController::getConversionProgress, "/api/conversion/{id}/progress", Get);
    ADD_METHOD_TO(ConversionController::downloadConvertedFile, "/api/conversion/{id}/download", Get);
    ADD_METHOD_TO(ConversionController::cancelConversion, "/api/conversion/{id}", Delete);
    ADD_METHOD_TO(ConversionController::getSupportedFormats, "/api/formats", Get);
    ADD_METHOD_TO(ConversionController::convertFile, "/api/convert", Post);
    METHOD_LIST_END
    
    void startConversion(const HttpRequestPtr& req,
                        std::function<void(const HttpResponsePtr&)>&& callback);
    
    void getConversionStatus(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback,
                            const std::string& id);
    
    void getConversionProgress(const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& callback,
                              const std::string& id);
    
    void downloadConvertedFile(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback,
                               const std::string& id);
    
    void cancelConversion(const HttpRequestPtr& req,
                         std::function<void(const HttpResponsePtr&)>&& callback,
                         const std::string& id);
    
    void getSupportedFormats(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback);
    
    void convertFile(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback);
    
private:
    std::string extractUserIdFromRequest(const HttpRequestPtr& req);
    std::shared_ptr<application::usecases::conversion::StartConversionUseCase> getStartConversionUseCase();
    std::shared_ptr<application::usecases::conversion::GetConversionProgressUseCase> getGetProgressUseCase();
    std::shared_ptr<domain::services::IConversionService> getConversionService();
    
    // Store conversion tasks: taskId -> outputFilePath
    std::map<std::string, std::string> conversionTasks_;
    std::mutex tasksMutex_;
};

} // namespace controllers
} // namespace presentation