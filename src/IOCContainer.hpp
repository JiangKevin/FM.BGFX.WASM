
#pragma once

#include <string>
#include <any>
#include <unordered_map>

namespace IOCContainer {
class Container {
private:
    Container() = default;
    
    // Disallow copying of object to ensure we don't get accidental copies of our IOCContainer
    Container(const Container&) = delete;
    void operator=(const Container&) = delete;
public:
    std::unordered_map<std::string, std::any> _container;
    
    static Container& get() {
        static Container instance;
        return instance;
    }
};

template<typename T> void registerInstance(T instance, const std::string& tag) {
    Container::get()._container[tag] = std::make_any<T>(instance);
}

template<typename T> void registerInstance(T instance) {
    Container::get()._container[T::k_iocTag] = std::make_any<T>(instance);
}

template<typename T> T& getInstance(const std::string& tag) {
    return std::any_cast<T&>(Container::get()._container[tag]);
}

template<typename T> T& getInstance() {
    return std::any_cast<T&>(Container::get()._container[T::k_iocTag]);
}

void unregisterInstances();
void unregisterInstance(const std::string& tag);
}