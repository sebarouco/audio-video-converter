#include "application/services/DependencyContainer.h"

namespace application {
namespace services {

DependencyContainer& getDependencyContainer() {
    static DependencyContainer instance;
    return instance;
}

} // namespace services
} // namespace application