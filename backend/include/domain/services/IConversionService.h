#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "domain/valueobjects/FileFormat.h"
#include "domain/valueobjects/ConversionStatus.h"

namespace domain {
namespace services {

struct ConversionRequest {
    std::string taskId;
    std::string inputFilePath;
    std::string outputFilePath;
    valueobjects::FileFormat sourceFormat;
    valueobjects::FileFormat targetFormat;
    std::vector<std::string> additionalOptions;
    
    ConversionRequest() = default;
    ConversionRequest(const std::string& id, const std::string& input, const std::string& output,
                    const valueobjects::FileFormat& source, const valueobjects::FileFormat& target,
                    const std::vector<std::string>& options = {})
        : taskId(id), inputFilePath(input), outputFilePath(output),
          sourceFormat(source), targetFormat(target), additionalOptions(options) {}
};

struct ConversionResult {
    bool success;
    std::string outputFilePath;
    std::string errorMessage;
    size_t outputFileSize;
    std::chrono::system_clock::time_point completedAt;
};

struct ConversionProgress {
    std::string taskId;
    int progress;
    std::string currentOperation;
    double speed;
    std::string eta;
    valueobjects::ConversionStatus status;
};

class IConversionService {
public:
    virtual ~IConversionService() = default;
    
    virtual ConversionResult convert(const ConversionRequest& request) = 0;
    virtual ConversionProgress getProgress(const std::string& taskId) = 0;
    virtual void cancelConversion(const std::string& taskId) = 0;
    virtual std::vector<valueobjects::FileFormat> getSupportedFormats() = 0;
    virtual bool canConvert(const valueobjects::FileFormat& source, const valueobjects::FileFormat& target) = 0;
    virtual std::vector<std::string> getConversionOptions(const valueobjects::FileFormat& source, 
                                                          const valueobjects::FileFormat& target) = 0;
};

} // namespace services
} // namespace domain