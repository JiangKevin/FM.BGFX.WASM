#include "eventDispatcher.hpp"

namespace events {

void registerEvent(EventDispatcher& dispatcher, const EventKey& eventKey, const EventHandler& handler) {
    dispatcher._events[eventKey] = handler;
}

void unregisterEvent(EventDispatcher& dispatcher, const EventKey& eventToRemove) {
    dispatcher._events.erase(dispatcher._events.find(eventToRemove));
}

void postEvent(EventDispatcher& dispatcher, const EventKey& eventToDispatch) {
    dispatcher._events[eventToDispatch]();
}
}