#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <stdexcept>

namespace application {
namespace services {

class DependencyContainer {
private:
    std::unordered_map<std::string, std::shared_ptr<void>> services_;
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> factories_;
    
public:
    DependencyContainer() = default;
    ~DependencyContainer() = default;
    
    template<typename T>
    void registerSingleton(std::shared_ptr<T> instance) {
        std::string typeName = typeid(T).name();
        services_[typeName] = instance;
    }
    
    template<typename T>
    void registerTransient(std::function<std::shared_ptr<T>()> factory) {
        std::string typeName = typeid(T).name();
        factories_[typeName] = [factory]() -> std::shared_ptr<void> {
            return factory();
        };
    }
    
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::string typeName = typeid(T).name();
        
        // Check if singleton exists
        auto it = services_.find(typeName);
        if (it != services_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        
        // Check if factory exists
        auto factoryIt = factories_.find(typeName);
        if (factoryIt != factories_.end()) {
            auto instance = factoryIt->second();
            services_[typeName] = instance;
            return std::static_pointer_cast<T>(instance);
        }
        
        throw std::runtime_error("Dependency not found: " + typeName);
    }
    
    template<typename T>
    bool isRegistered() const {
        std::string typeName = typeid(T).name();
        return services_.find(typeName) != services_.end() || 
               factories_.find(typeName) != factories_.end();
    }
    
    void clear() {
        services_.clear();
        factories_.clear();
    }
};

// Global instance
DependencyContainer& getDependencyContainer();

} // namespace services
} // namespace application