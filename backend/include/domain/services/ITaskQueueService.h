#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <chrono>

namespace domain {
namespace services {

enum class TaskPriority {
    LOW,
    NORMAL,
    HIGH,
    URGENT
};

struct Task {
    std::string id;
    std::string type;
    std::string payload;
    TaskPriority priority;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point scheduledFor;
    int retryCount;
    int maxRetries;
};

class ITaskQueueService {
public:
    virtual ~ITaskQueueService() = default;
    
    virtual std::string enqueueTask(const Task& task) = 0;
    virtual std::string enqueueTask(const Task& task, TaskPriority priority) = 0;
    virtual std::string enqueueDelayedTask(const Task& task, int delaySeconds) = 0;
    virtual std::optional<Task> dequeueTask() = 0;
    virtual std::optional<Task> dequeueTask(const std::string& queueName) = 0;
    virtual void updateTaskStatus(const std::string& taskId, const std::string& status) = 0;
    virtual void updateTaskProgress(const std::string& taskId, int progress) = 0;
    virtual std::vector<Task> getPendingTasks() = 0;
    virtual std::vector<Task> getPendingTasks(const std::string& queueName) = 0;
    virtual std::optional<Task> getTask(const std::string& taskId) = 0;
    virtual void deleteTask(const std::string& taskId) = 0;
    virtual void clearQueue() = 0;
    virtual void clearQueue(const std::string& queueName) = 0;
    virtual size_t getQueueSize() = 0;
    virtual size_t getQueueSize(const std::string& queueName) = 0;
    
    // For async processing
    virtual void startWorker(std::function<void(const Task&)> processor) = 0;
    virtual void stopWorker() = 0;
    virtual bool isWorkerRunning() = 0;
};

} // namespace services
} // namespace domain