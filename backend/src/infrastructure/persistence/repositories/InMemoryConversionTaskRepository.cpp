#include "infrastructure/persistence/repositories/InMemoryConversionTaskRepository.h"
#include <spdlog/spdlog.h>

namespace infrastructure {
namespace persistence {
namespace repositories {

std::optional<domain::entities::ConversionTask> InMemoryConversionTaskRepository::findById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = taskMap_.find(id);
    if (it != taskMap_.end()) {
        return *it->second;
    }
    return std::nullopt;
}

std::vector<domain::entities::ConversionTask> InMemoryConversionTaskRepository::findByUserId(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::entities::ConversionTask> tasks;
    for (const auto& pair : taskMap_) {
        if (pair.second->getUserId() == userId) {
            tasks.push_back(*pair.second);
        }
    }
    return tasks;
}

std::vector<domain::entities::ConversionTask> InMemoryConversionTaskRepository::findByStatus(
    const domain::valueobjects::ConversionStatus& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::entities::ConversionTask> tasks;
    for (const auto& pair : taskMap_) {
        if (pair.second->getStatus() == status) {
            tasks.push_back(*pair.second);
        }
    }
    return tasks;
}

std::vector<domain::entities::ConversionTask> InMemoryConversionTaskRepository::findByUserIdAndStatus(
    const std::string& userId, 
    const domain::valueobjects::ConversionStatus& status) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::entities::ConversionTask> tasks;
    for (const auto& pair : taskMap_) {
        if (pair.second->getUserId() == userId && pair.second->getStatus() == status) {
            tasks.push_back(*pair.second);
        }
    }
    return tasks;
}

std::vector<domain::entities::ConversionTask> InMemoryConversionTaskRepository::findActiveTasks(int limit) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<domain::entities::ConversionTask> tasks;
    int count = 0;
    for (const auto& pair : taskMap_) {
        if (count >= limit) break;
        const auto& status = pair.second->getStatus();
        if (status == domain::valueobjects::ConversionStatus::PENDING || 
            status == domain::valueobjects::ConversionStatus::PROCESSING ||
            status == domain::valueobjects::ConversionStatus::QUEUED) {
            tasks.push_back(*pair.second);
            count++;
        }
    }
    return tasks;
}

void InMemoryConversionTaskRepository::save(const domain::entities::ConversionTask& task) {
    std::lock_guard<std::mutex> lock(mutex_);
    taskMap_[task.getId()] = std::make_shared<domain::entities::ConversionTask>(task);
    spdlog::info("Saved conversion task: {}", task.getId());
}

void InMemoryConversionTaskRepository::update(const domain::entities::ConversionTask& task) {
    std::lock_guard<std::mutex> lock(mutex_);
    taskMap_[task.getId()] = std::make_shared<domain::entities::ConversionTask>(task);
    spdlog::info("Updated conversion task: {}", task.getId());
}

void InMemoryConversionTaskRepository::deleteById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    taskMap_.erase(id);
    spdlog::info("Deleted conversion task: {}", id);
}

bool InMemoryConversionTaskRepository::existsById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return taskMap_.find(id) != taskMap_.end();
}

size_t InMemoryConversionTaskRepository::count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return taskMap_.size();
}

size_t InMemoryConversionTaskRepository::countByUserId(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& pair : taskMap_) {
        if (pair.second->getUserId() == userId) {
            count++;
        }
    }
    return count;
}

} // namespace repositories
} // namespace persistence
} // namespace infrastructure