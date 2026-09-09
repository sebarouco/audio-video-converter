#pragma once

#include <string>

namespace domain {
namespace valueobjects {

enum class ConversionStatus {
    PENDING,
    QUEUED,
    PROCESSING,
    COMPLETED,
    FAILED,
    CANCELLED,
    TIMEOUT
};

class ConversionStatusHelper {
public:
    static std::string toString(ConversionStatus status);
    static ConversionStatus fromString(const std::string& status);
    static bool isTerminal(ConversionStatus status);
    static bool isActive(ConversionStatus status);
};

} // namespace valueobjects
} // namespace domain