#pragma once

#include <string>
#include <functional>
#include <unordered_map>

namespace events {

using EventHandler = std::function<void()>;
using EventKey = std::string;

struct EventDispatcher {
    inline static const std::string k_iocTag = "eventHandlerTag";
    std::unordered_map<EventKey, EventHandler> _events;
};

void registerEvent(EventDispatcher& dispatcher, const EventKey& eventKey, const EventHandler& handler);
void unregisterEvent(EventDispatcher& dispatcher, const EventKey& eventToRemove);
void postEvent(EventDispatcher& dispatcher, const EventKey& eventToDispatch);

}