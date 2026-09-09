#pragma once

#include "domain/repositories/IConversionTaskRepository.h"
#include <unordered_map>
#include <mutex>
#include <vector>

namespace infrastructure {
namespace persistence {
namespace repositories {

class InMemoryConversionTaskRepository : public domain::repositories::IConversionTaskRepository {
private:
    std::unordered_map<std::string, std::shared_ptr<domain::entities::ConversionTask>> taskMap_;
    mutable std::mutex mutex_;

public:
    InMemoryConversionTaskRepository() = default;
    ~InMemoryConversionTaskRepository() override = default;

    std::optional<domain::entities::ConversionTask> findById(const std::string& id) override;
    std::vector<domain::entities::ConversionTask> findByUserId(const std::string& userId) override;
    std::vector<domain::entities::ConversionTask> findByStatus(
        const domain::valueobjects::ConversionStatus& status) override;
    std::vector<domain::entities::ConversionTask> findByUserIdAndStatus(
        const std::string& userId, 
        const domain::valueobjects::ConversionStatus& status) override;
    std::vector<domain::entities::ConversionTask> findActiveTasks(int limit = 100) override;
    void save(const domain::entities::ConversionTask& task) override;
    void update(const domain::entities::ConversionTask& task) override;
    void deleteById(const std::string& id) override;
    bool existsById(const std::string& id) override;
    size_t count() override;
    size_t countByUserId(const std::string& userId) override;
};

} // namespace repositories
} // namespace persistence
} // namespace infrastructure