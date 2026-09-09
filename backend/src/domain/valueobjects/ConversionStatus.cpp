#include "domain/valueobjects/ConversionStatus.h"
#include <unordered_map>
#include <spdlog/spdlog.h>

namespace domain {
namespace valueobjects {

std::string ConversionStatusHelper::toString(ConversionStatus status) {
    static const std::unordered_map<ConversionStatus, std::string> statusMap = {
        {ConversionStatus::PENDING, "PENDING"},
        {ConversionStatus::QUEUED, "QUEUED"},
        {ConversionStatus::PROCESSING, "PROCESSING"},
        {ConversionStatus::COMPLETED, "COMPLETED"},
        {ConversionStatus::FAILED, "FAILED"},
        {ConversionStatus::CANCELLED, "CANCELLED"},
        {ConversionStatus::TIMEOUT, "TIMEOUT"}
    };
    
    auto it = statusMap.find(status);
    if (it != statusMap.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

ConversionStatus ConversionStatusHelper::fromString(const std::string& status) {
    static const std::unordered_map<std::string, ConversionStatus> stringMap = {
        {"PENDING", ConversionStatus::PENDING},
        {"QUEUED", ConversionStatus::QUEUED},
        {"PROCESSING", ConversionStatus::PROCESSING},
        {"COMPLETED", ConversionStatus::COMPLETED},
        {"FAILED", ConversionStatus::FAILED},
        {"CANCELLED", ConversionStatus::CANCELLED},
        {"TIMEOUT", ConversionStatus::TIMEOUT}
    };
    
    auto it = stringMap.find(status);
    if (it != stringMap.end()) {
        return it->second;
    }
    return ConversionStatus::PENDING; // Default fallback
}

bool ConversionStatusHelper::isTerminal(ConversionStatus status) {
    return status == ConversionStatus::COMPLETED ||
           status == ConversionStatus::FAILED ||
           status == ConversionStatus::CANCELLED ||
           status == ConversionStatus::TIMEOUT;
}

bool ConversionStatusHelper::isActive(ConversionStatus status) {
    return status == ConversionStatus::PENDING ||
           status == ConversionStatus::QUEUED ||
           status == ConversionStatus::PROCESSING;
}

} // namespace valueobjects
} // namespace domain