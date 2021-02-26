#ifndef KUGE_HEADERS_EVENT_BUS_H_
#define KUGE_HEADERS_EVENT_BUS_H_

#include "event.h"
#include <memory>
#include <queue>
#include <vector>

namespace kuge {

class System;

class EventBus {
 public:
  void postEvent(const Event& event)
    { events_.push({event}); }
  void postEvent(EventTypes event_t)
    { events_.push({event_t}); }
  void postEvent(EventTypes event_t, float value)
    { events_.push({event_t, value}); }
  void postEvent(EventTypes event_t, const std::string& msg)
    { events_.push({event_t, msg}); }
  void postEvent(EventTypes event_t, float value, const std::string& msg)
    { events_.push({event_t, value, msg}); }
  void postImmediateEvent(const Event& event) const;
  void postImmediateEvent(EventTypes event_t) const;
  void postImmediateEvent(EventTypes event_t, float value) const;
  void postImmediateEvent(EventTypes event_t, const std::string& msg) const;
  void postImmediateEvent(EventTypes event_t, float value, const std::string& msg) const;
  void processEvents();
  
  template<typename T> 
  void setSystems(const T& t) {
    systems_.push_back(std::make_unique<T>(t));
  }

  template<typename T, typename... Args> 
  void setSystems(const T& t, Args... args) { 
    systems_.push_back(std::make_unique<T>(t));
    setSystems(args...) ;
  }

 private:
  std::queue<Event> events_{};
  std::vector<std::unique_ptr<System>> systems_{};
};

} // end namespace kuge

#endif // KUGE_HEADERS_EVENT_BUS_H_