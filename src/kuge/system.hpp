#ifndef KUGE_HEADERS_SYSTEM_HPP_
#define KUGE_HEADERS_SYSTEM_HPP_

#include "event.hpp"

namespace kuge {

class System {
 public:
  virtual ~System() {}
  virtual void handleEvent(const KugeEvent*) = 0;
  /* inline void postEvent(KugeEvent& event) const {
    event_bus_.postEvent(event);
  } */
 protected:
  static const EventBus* event_bus_;
};

class InputSystem: public System {
 public:
  virtual void handleEvent(const KugeEvent* event) override {}
};

class OutputSystem: public System {
 public:
  OutputSystem(bool log): log_(log) {}
  virtual inline void handleEvent(const KugeEvent* event) override { if (log_) event->print(); }
  inline void setLog(bool status) { log_ = status; }
 private:
  bool log_;
};

} // end namespace kuge

#endif // KUGE_HEADERS_SYSTEM_HPP_
