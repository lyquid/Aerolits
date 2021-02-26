#ifndef KUGE_HEADERS_SYSTEM_H_
#define KUGE_HEADERS_SYSTEM_H_

#include "./event.h"
#include <string>

namespace kuge {

class EventBus;

class System {
 public:
  System(EventBus& bus): event_bus_(bus) {}
  virtual ~System() {}
  virtual void handleEvent(const Event& event) = 0;
  void postEvent(Event event) 
    { event_bus_.postEvent(event); }
  void postEvent(EventTypes event_t) 
    { event_bus_.postEvent(event_t); }
  void postEvent(EventTypes event_t, float value) 
    { event_bus_.postEvent(event_t, value); }
  void postEvent(EventTypes event_t, const std::string& msg) 
    { event_bus_.postEvent(event_t, msg); }
  void postEvent(EventTypes event_t, float value, const std::string& msg) 
    { event_bus_.postEvent(event_t, value, msg); }

 private:
  EventBus& event_bus_;
};

} // end namespace kuge

#endif // KUGE_HEADERS_SYSTEM_H_