#include "domain/entities/ConversionTask.h"
#include <spdlog/spdlog.h>

namespace domain {
namespace entities {

ConversionTask::ConversionTask(std::string id, std::string userId, std::string originalFilePath,
                              valueobjects::FileFormat sourceFormat, valueobjects::FileFormat targetFormat)
    : id_(std::move(id)),
      userId_(std::move(userId)),
      originalFilePath_(std::move(originalFilePath)),
      convertedFilePath_(""),
      sourceFormat_(std::move(sourceFormat)),
      targetFormat_(std::move(targetFormat)),
      status_(valueobjects::ConversionStatus::PENDING),
      progress_(0),
      errorMessage_(""),
      createdAt_(std::chrono::system_clock::now()),
      startedAt_(std::chrono::system_clock::time_point{}),
      completedAt_(std::chrono::system_clock::time_point{}) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid conversion task parameters");
    }
}

ConversionTask::ConversionTask(const ConversionTask& other)
    : id_(other.id_),
      userId_(other.userId_),
      originalFilePath_(other.originalFilePath_),
      convertedFilePath_(other.convertedFilePath_),
      sourceFormat_(other.sourceFormat_),
      targetFormat_(other.targetFormat_),
      status_(other.status_),
      progress_(other.progress_),
      errorMessage_(other.errorMessage_),
      createdAt_(other.createdAt_),
      startedAt_(other.startedAt_),
      completedAt_(other.completedAt_) {
}

ConversionTask& ConversionTask::operator=(const ConversionTask& other) {
    if (this != &other) {
        id_ = other.id_;
        userId_ = other.userId_;
        originalFilePath_ = other.originalFilePath_;
        convertedFilePath_ = other.convertedFilePath_;
        sourceFormat_ = other.sourceFormat_;
        targetFormat_ = other.targetFormat_;
        status_ = other.status_;
        progress_ = other.progress_;
        errorMessage_ = other.errorMessage_;
        createdAt_ = other.createdAt_;
        startedAt_ = other.startedAt_;
        completedAt_ = other.completedAt_;
    }
    return *this;
}

ConversionTask::ConversionTask(ConversionTask&& other) noexcept
    : id_(std::move(other.id_)),
      userId_(std::move(other.userId_)),
      originalFilePath_(std::move(other.originalFilePath_)),
      convertedFilePath_(std::move(other.convertedFilePath_)),
      sourceFormat_(std::move(other.sourceFormat_)),
      targetFormat_(std::move(other.targetFormat_)),
      status_(other.status_),
      progress_(other.progress_),
      errorMessage_(std::move(other.errorMessage_)),
      createdAt_(other.createdAt_),
      startedAt_(other.startedAt_),
      completedAt_(other.completedAt_) {
}

ConversionTask& ConversionTask::operator=(ConversionTask&& other) noexcept {
    if (this != &other) {
        id_ = std::move(other.id_);
        userId_ = std::move(other.userId_);
        originalFilePath_ = std::move(other.originalFilePath_);
        convertedFilePath_ = std::move(other.convertedFilePath_);
        sourceFormat_ = std::move(other.sourceFormat_);
        targetFormat_ = std::move(other.targetFormat_);
        status_ = other.status_;
        progress_ = other.progress_;
        errorMessage_ = std::move(other.errorMessage_);
        createdAt_ = other.createdAt_;
        startedAt_ = other.startedAt_;
        completedAt_ = other.completedAt_;
    }
    return *this;
}

void ConversionTask::updateProgress(int progress) {
    if (progress < 0 || progress > 100) {
        throw std::invalid_argument("Progress must be between 0 and 100");
    }
    progress_ = progress;
    
    if (progress > 0 && status_ == valueobjects::ConversionStatus::PENDING) {
        status_ = valueobjects::ConversionStatus::PROCESSING;
    }
}

void ConversionTask::markAsStarted() {
    status_ = valueobjects::ConversionStatus::PROCESSING;
    startedAt_ = std::chrono::system_clock::now();
    progress_ = 0;
}

void ConversionTask::markAsCompleted(const std::string& convertedFilePath) {
    status_ = valueobjects::ConversionStatus::COMPLETED;
    if (!convertedFilePath.empty()) {
        convertedFilePath_ = convertedFilePath;
    }
    completedAt_ = std::chrono::system_clock::now();
    progress_ = 100;
    errorMessage_ = "";
}

void ConversionTask::markAsFailed(const std::string& error) {
    status_ = valueobjects::ConversionStatus::FAILED;
    errorMessage_ = error;
    completedAt_ = std::chrono::system_clock::now();
}

void ConversionTask::markAsCancelled() {
    status_ = valueobjects::ConversionStatus::CANCELLED;
    completedAt_ = std::chrono::system_clock::now();
}

bool ConversionTask::isValid() const {
    return !id_.empty() && 
           !userId_.empty() && 
           !originalFilePath_.empty() &&
           sourceFormat_.isSupported() &&
           targetFormat_.isSupported();
}

bool ConversionTask::canBeCancelled() const {
    return status_ == valueobjects::ConversionStatus::PENDING ||
           status_ == valueobjects::ConversionStatus::QUEUED ||
           status_ == valueobjects::ConversionStatus::PROCESSING;
}

bool ConversionTask::isCompleted() const {
    return status_ == valueobjects::ConversionStatus::COMPLETED;
}

bool ConversionTask::isFailed() const {
    return status_ == valueobjects::ConversionStatus::FAILED ||
           status_ == valueobjects::ConversionStatus::CANCELLED ||
           status_ == valueobjects::ConversionStatus::TIMEOUT;
}

} // namespace entities
} // namespace domain