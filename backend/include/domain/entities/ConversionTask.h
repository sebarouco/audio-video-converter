#pragma once

#include <string>
#include <chrono>
#include "domain/valueobjects/FileFormat.h"
#include "domain/valueobjects/ConversionStatus.h"

namespace domain {
namespace entities {

class ConversionTask {
private:
    std::string id_;
    std::string userId_;
    std::string originalFilePath_;
    std::string convertedFilePath_;
    valueobjects::FileFormat sourceFormat_;
    valueobjects::FileFormat targetFormat_;
    valueobjects::ConversionStatus status_;
    int progress_;
    std::string errorMessage_;
    std::chrono::system_clock::time_point createdAt_;
    std::chrono::system_clock::time_point startedAt_;
    std::chrono::system_clock::time_point completedAt_;

public:
    ConversionTask(std::string id, std::string userId, std::string originalFilePath,
                   valueobjects::FileFormat sourceFormat, valueobjects::FileFormat targetFormat);
    
    // Copy constructor
    ConversionTask(const ConversionTask& other);
    
    // Copy assignment
    ConversionTask& operator=(const ConversionTask& other);
    
    // Move constructor
    ConversionTask(ConversionTask&& other) noexcept;
    
    // Move assignment
    ConversionTask& operator=(ConversionTask&& other) noexcept;
    
    // Getters
    std::string getId() const { return id_; }
    std::string getUserId() const { return userId_; }
    std::string getInputFilePath() const { return originalFilePath_; }
    std::string getOriginalFilePath() const { return originalFilePath_; }
    std::string getConvertedFilePath() const { return convertedFilePath_; }
    std::string getOutputFilePath() const { return convertedFilePath_; }
    valueobjects::FileFormat getSourceFormat() const { return sourceFormat_; }
    valueobjects::FileFormat getTargetFormat() const { return targetFormat_; }
    valueobjects::ConversionStatus getStatus() const { return status_; }
    int getProgress() const { return progress_; }
    std::string getErrorMessage() const { return errorMessage_; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt_; }
    std::chrono::system_clock::time_point getStartedAt() const { return startedAt_; }
    std::chrono::system_clock::time_point getCompletedAt() const { return completedAt_; }
    
    // Setters
    void setOutputFilePath(const std::string& path) { convertedFilePath_ = path; }
    
    // State management
    void updateProgress(int progress);
    void markAsStarted();
    void markAsCompleted(const std::string& convertedFilePath = "");
    void markAsFailed(const std::string& error);
    void markAsCancelled();
    
    // Validation
    bool isValid() const;
    bool canBeCancelled() const;
    bool isCompleted() const;
    bool isFailed() const;
};

} // namespace entities
} // namespace domain