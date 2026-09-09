#include "infrastructure/services/queue/RedisTaskQueueService.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include <random>
#include <sstream>

namespace infrastructure {
namespace services {
namespace queue {

RedisTaskQueueService::RedisTaskQueueService(const std::string& host,
                                             int port,
                                             const std::string& password,
                                             int db)
    : workerRunning_(false) {
    // Store connection parameters for future Redis implementation
    // For now, using in-memory mode
    spdlog::info("RedisTaskQueueService initialized (in-memory mode) with host: {}, port: {}", host, port);
}

RedisTaskQueueService::~RedisTaskQueueService() {
    stopWorker();
}

std::string RedisTaskQueueService::generateTaskId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
        if (i == 8 || i == 12 || i == 16 || i == 20) ss << '-';
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

void RedisTaskQueueService::workerLoop() {
    spdlog::info("Task queue worker started");
    
    while (workerRunning_) {
        try {
            std::unique_lock<std::mutex> lock(queueMutex_);
            
            if (taskQueue_.empty()) {
                queueCondition_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            auto task = taskQueue_.front();
            taskQueue_.pop();
            lock.unlock();
            
            if (taskProcessor_) {
                taskProcessor_(task);
            }
            
        } catch (const std::exception& e) {
            spdlog::error("Error in worker loop: {}", e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    spdlog::info("Task queue worker stopped");
}

std::string RedisTaskQueueService::enqueueTask(const domain::services::Task& task) {
    return enqueueTask(task, domain::services::TaskPriority::NORMAL);
}

std::string RedisTaskQueueService::enqueueTask(const domain::services::Task& task, domain::services::TaskPriority priority) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    try {
        domain::services::Task newTask = task;
        if (newTask.id.empty()) {
            newTask.id = generateTaskId();
        }
        
        taskQueue_.push(newTask);
        taskMap_[newTask.id] = newTask;
        taskStatuses_[newTask.id] = "pending";
        taskProgress_[newTask.id] = 0;
        
        queueCondition_.notify_one();
        
        spdlog::info("Task enqueued successfully: {}", newTask.id);
        return newTask.id;
        
    } catch (const std::exception& e) {
        spdlog::error("Error enqueuing task: {}", e.what());
        return "";
    }
}

std::string RedisTaskQueueService::enqueueDelayedTask(const domain::services::Task& task, int delaySeconds) {
    // For simplicity, treat delayed tasks as normal tasks
    return enqueueTask(task);
}

std::optional<domain::services::Task> RedisTaskQueueService::dequeueTask() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    if (taskQueue_.empty()) {
        return std::nullopt;
    }
    
    auto task = taskQueue_.front();
    taskQueue_.pop();
    
    spdlog::info("Task dequeued successfully: {}", task.id);
    return task;
}

std::optional<domain::services::Task> RedisTaskQueueService::dequeueTask(const std::string& queueName) {
    return dequeueTask();
}

void RedisTaskQueueService::updateTaskStatus(const std::string& taskId, const std::string& status) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    taskStatuses_[taskId] = status;
    spdlog::info("Task status updated: {} -> {}", taskId, status);
}

void RedisTaskQueueService::updateTaskProgress(const std::string& taskId, int progress) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    taskProgress_[taskId] = progress;
}

std::vector<domain::services::Task> RedisTaskQueueService::getPendingTasks() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    std::vector<domain::services::Task> tasks;
    std::queue<domain::services::Task> tempQueue = taskQueue_;
    
    while (!tempQueue.empty()) {
        tasks.push_back(tempQueue.front());
        tempQueue.pop();
    }
    
    return tasks;
}

std::vector<domain::services::Task> RedisTaskQueueService::getPendingTasks(const std::string& queueName) {
    return getPendingTasks();
}

std::optional<domain::services::Task> RedisTaskQueueService::getTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    auto it = taskMap_.find(taskId);
    if (it != taskMap_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

void RedisTaskQueueService::deleteTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    taskMap_.erase(taskId);
    taskStatuses_.erase(taskId);
    taskProgress_.erase(taskId);
    
    spdlog::info("Task deleted: {}", taskId);
}

void RedisTaskQueueService::clearQueue() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    while (!taskQueue_.empty()) {
        taskQueue_.pop();
    }
    
    taskMap_.clear();
    taskStatuses_.clear();
    taskProgress_.clear();
    
    spdlog::info("Queue cleared");
}

void RedisTaskQueueService::clearQueue(const std::string& queueName) {
    clearQueue();
}

size_t RedisTaskQueueService::getQueueSize() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return taskQueue_.size();
}

size_t RedisTaskQueueService::getQueueSize(const std::string& queueName) {
    return getQueueSize();
}

void RedisTaskQueueService::startWorker(std::function<void(const domain::services::Task&)> processor) {
    if (workerRunning_) {
        spdlog::warn("Worker is already running");
        return;
    }
    
    taskProcessor_ = processor;
    workerRunning_ = true;
    workerThread_ = std::thread(&RedisTaskQueueService::workerLoop, this);
}

void RedisTaskQueueService::stopWorker() {
    if (!workerRunning_) {
        return;
    }
    
    workerRunning_ = false;
    queueCondition_.notify_all();
    
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
}

bool RedisTaskQueueService::isWorkerRunning() {
    return workerRunning_;
}

} // namespace queue
} // namespace services
} // namespace infrastructure