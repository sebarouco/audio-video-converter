#pragma once

#include <string>
#include <vector>
#include <optional>
#include "domain/entities/User.h"

namespace domain {
namespace repositories {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    
    virtual std::optional<entities::User> findById(const std::string& id) = 0;
    virtual std::optional<entities::User> findByEmail(const std::string& email) = 0;
    virtual std::vector<entities::User> findAll(int limit = 100, int offset = 0) = 0;
    virtual void save(const entities::User& user) = 0;
    virtual void update(const entities::User& user) = 0;
    virtual void deleteById(const std::string& id) = 0;
    virtual bool existsById(const std::string& id) = 0;
    virtual bool existsByEmail(const std::string& email) = 0;
    virtual size_t count() = 0;
};

} // namespace repositories
} // namespace domain