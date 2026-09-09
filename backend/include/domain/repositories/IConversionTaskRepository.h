#pragma once

#include <string>
#include <vector>
#include <optional>
#include "domain/entities/ConversionTask.h"

namespace domain {
namespace repositories {

class IConversionTaskRepository {
public:
    virtual ~IConversionTaskRepository() = default;
    
    virtual std::optional<entities::ConversionTask> findById(const std::string& id) = 0;
    virtual std::vector<entities::ConversionTask> findByUserId(const std::string& userId) = 0;
    virtual std::vector<entities::ConversionTask> findByStatus(
        const valueobjects::ConversionStatus& status) = 0;
    virtual std::vector<entities::ConversionTask> findByUserIdAndStatus(
        const std::string& userId, 
        const valueobjects::ConversionStatus& status) = 0;
    virtual std::vector<entities::ConversionTask> findActiveTasks(int limit = 100) = 0;
    virtual void save(const entities::ConversionTask& task) = 0;
    virtual void update(const entities::ConversionTask& task) = 0;
    virtual void deleteById(const std::string& id) = 0;
    virtual bool existsById(const std::string& id) = 0;
    virtual size_t count() = 0;
    virtual size_t countByUserId(const std::string& userId) = 0;
};

} // namespace repositories
} // namespace domain