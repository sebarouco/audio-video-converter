#pragma once

#include <string>
#include <optional>
#include <stdexcept>

namespace application {
namespace interfaces {

template<typename TRequest, typename TResponse>
class IUseCase {
public:
    virtual ~IUseCase() = default;
    
    virtual TResponse execute(const TRequest& request) = 0;
    virtual bool validateRequest(const TRequest& request) const = 0;
    virtual std::string getUseCaseName() const = 0;
};

class UseCaseException : public std::runtime_error {
public:
    explicit UseCaseException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ValidationException : public UseCaseException {
public:
    explicit ValidationException(const std::string& message) 
        : UseCaseException(message) {}
};

class AuthorizationException : public UseCaseException {
public:
    explicit AuthorizationException(const std::string& message) 
        : UseCaseException(message) {}
};

class ResourceNotFoundException : public UseCaseException {
public:
    explicit ResourceNotFoundException(const std::string& message) 
        : UseCaseException(message) {}
};

} // namespace interfaces
} // namespace application