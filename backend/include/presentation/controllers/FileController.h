#pragma once

#include <drogon/HttpController.h>
#include "application/usecases/conversion/UploadFileUseCase.h"
#include "application/dto/ConversionTaskDTO.h"

using namespace drogon;

namespace presentation {
namespace controllers {

class FileController : public drogon::HttpController<FileController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadFile, "/api/files/upload", Post);
    ADD_METHOD_TO(FileController::getFileInfo, "/api/files/{id}", Get);
    ADD_METHOD_TO(FileController::deleteFile, "/api/files/{id}", Delete);
    METHOD_LIST_END
    
    void uploadFile(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);
    
    void getFileInfo(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback,
                     const std::string& id);
    
    void deleteFile(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback,
                    const std::string& id);
    
private:
    std::shared_ptr<application::usecases::conversion::UploadFileUseCase> uploadUseCase_;
    
    void validateFileUpload(const HttpRequestPtr& req);
};

} // namespace controllers
} // namespace presentation