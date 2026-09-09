#pragma once

#include "domain/services/ITaskQueueService.h"
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace infrastructure {
namespace services {
namespace queue {

class RedisTaskQueueService : public domain::services::ITaskQueueService {
private:
    std::queue<domain::services::Task> taskQueue_;
    std::unordered_map<std::string, domain::services::Task> taskMap_;
    std::unordered_map<std::string, std::string> taskStatuses_;
    std::unordered_map<std::string, int> taskProgress_;
    
    mutable std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    
    std::thread workerThread_;
    std::atomic<bool> workerRunning_;
    std::function<void(const domain::services::Task&)> taskProcessor_;
    
    std::string generateTaskId() const;
    void workerLoop();
    
public:
    RedisTaskQueueService(const std::string& host = "localhost",
                         int port = 6379,
                         const std::string& password = "",
                         int db = 0);
    ~RedisTaskQueueService() override;
    
    std::string enqueueTask(const domain::services::Task& task) override;
    std::string enqueueTask(const domain::services::Task& task, domain::services::TaskPriority priority) override;
    std::string enqueueDelayedTask(const domain::services::Task& task, int delaySeconds) override;
    std::optional<domain::services::Task> dequeueTask() override;
    std::optional<domain::services::Task> dequeueTask(const std::string& queueName) override;
    void updateTaskStatus(const std::string& taskId, const std::string& status) override;
    void updateTaskProgress(const std::string& taskId, int progress) override;
    std::vector<domain::services::Task> getPendingTasks() override;
    std::vector<domain::services::Task> getPendingTasks(const std::string& queueName) override;
    std::optional<domain::services::Task> getTask(const std::string& taskId) override;
    void deleteTask(const std::string& taskId) override;
    void clearQueue() override;
    void clearQueue(const std::string& queueName) override;
    size_t getQueueSize() override;
    size_t getQueueSize(const std::string& queueName) override;
    
    void startWorker(std::function<void(const domain::services::Task&)> processor) override;
    void stopWorker() override;
    bool isWorkerRunning() override;
};

} // namespace queue
} // namespace services
} // namespace infrastructure