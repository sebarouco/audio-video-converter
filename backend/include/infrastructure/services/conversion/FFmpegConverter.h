#pragma once

#include "domain/services/IConversionService.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace infrastructure {
namespace services {
namespace conversion {

class FFmpegConverter : public domain::services::IConversionService {
private:
    std::string ffmpegPath_;
    int threads_;
    int timeoutSeconds_;
    
    std::unordered_map<std::string, domain::services::ConversionProgress> progressMap_;
    mutable std::mutex progressMutex_;
    
    std::string buildFFmpegCommand(const domain::services::ConversionRequest& request) const;
    void updateProgress(const std::string& taskId, const domain::services::ConversionProgress& progress);
    domain::services::ConversionProgress getProgressInternal(const std::string& taskId) const;
    void executeConversion(const domain::services::ConversionRequest& request);
    std::string parseProgress(const std::string& output) const;
    bool isConversionRunning(const std::string& taskId) const;
    
public:
    FFmpegConverter(const std::string& ffmpegPath = "/usr/bin/ffmpeg",
                    int threads = 2,
                    int timeoutSeconds = 3600);
    
    ~FFmpegConverter() override = default;
    
    domain::services::ConversionResult convert(const domain::services::ConversionRequest& request) override;
    domain::services::ConversionProgress getProgress(const std::string& taskId) override;
    void cancelConversion(const std::string& taskId) override;
    std::vector<domain::valueobjects::FileFormat> getSupportedFormats() override;
    bool canConvert(const domain::valueobjects::FileFormat& source, 
                    const domain::valueobjects::FileFormat& target) override;
    std::vector<std::string> getConversionOptions(const domain::valueobjects::FileFormat& source, 
                                                  const domain::valueobjects::FileFormat& target) override;
};

} // namespace conversion
} // namespace services
} // namespace infrastructure