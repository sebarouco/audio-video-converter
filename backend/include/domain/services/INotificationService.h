#pragma once

#include <string>
#include <functional>
#include "domain/services/IConversionService.h"

namespace domain {
namespace services {

struct Notification {
    std::string userId;
    std::string type;
    std::string title;
    std::string message;
    std::string data;
    std::chrono::system_clock::time_point createdAt;
};

class INotificationService {
public:
    virtual ~INotificationService() = default;
    
    virtual void notifyProgress(const std::string& userId, 
                               const ConversionProgress& progress) = 0;
    virtual void notifyCompletion(const std::string& userId, 
                                  const ConversionResult& result) = 0;
    virtual void notifyError(const std::string& userId, 
                            const std::string& error) = 0;
    virtual void notifyCancellation(const std::string& userId,
                                   const std::string& taskId) = 0;
    
    virtual void sendNotification(const Notification& notification) = 0;
    virtual void broadcastNotification(const Notification& notification) = 0;
    
    // WebSocket-specific
    virtual void subscribeToUserNotifications(const std::string& userId,
                                             std::function<void(const Notification&)> callback) = 0;
    virtual void unsubscribeFromUserNotifications(const std::string& userId) = 0;
};

} // namespace services
} // namespace domain