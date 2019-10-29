#include "IOCContainer.hpp"

namespace IOCContainer {
void unregisterInstances() {
    Container::get()._container.clear();
}

void unregisterInstance(const std::string& tag) {
    Container::get()._container.erase(tag);
}
}