#pragma once

#include "domain/repositories/IUserRepository.h"
#include "infrastructure/persistence/models/UserModel.h"
#include "infrastructure/external/DatabaseConnection.h"
#include <memory>
#include <optional>

namespace infrastructure {
namespace persistence {
namespace repositories {

class SqlUserRepository : public domain::repositories::IUserRepository {
private:
    std::shared_ptr<external::DatabaseConnection> dbConnection_;
    
    domain::entities::User modelToEntity(const models::UserModel& model) const;
    models::UserModel entityToModel(const domain::entities::User& entity) const;
    
public:
    explicit SqlUserRepository(std::shared_ptr<external::DatabaseConnection> dbConnection);
    
    ~SqlUserRepository() override = default;
    
    std::optional<domain::entities::User> findById(const std::string& id) override;
    std::optional<domain::entities::User> findByEmail(const std::string& email) override;
    std::vector<domain::entities::User> findAll(int limit, int offset) override;
    void save(const domain::entities::User& user) override;
    void update(const domain::entities::User& user) override;
    void deleteById(const std::string& id) override;
    bool existsById(const std::string& id) override;
    bool existsByEmail(const std::string& email) override;
    size_t count() override;
};

} // namespace repositories
} // namespace persistence
} // namespace infrastructure