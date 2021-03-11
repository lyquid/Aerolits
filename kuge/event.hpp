#ifndef KUGE_HEADERS_EVENT_H_
#define KUGE_HEADERS_EVENT_H_

#include "event_types.hpp"
#include <cmath>
#include <iostream>
#include <string>

namespace kuge {

class Event {
 public:
  Event(EventTypes type);
  Event(EventTypes type, float value);
  Event(EventTypes type, const std::string& msg);
  Event(EventTypes type, float value, const std::string& msg);
  ~Event() {}
  
  const std::string& getMessage() const { return kMessage_; }
  const std::string& getName()    const { return kName_; }
  EventTypes         getType()    const { return kType_; }
  float              getValue()   const { return kValue_; };

  friend std::ostream& operator<<(std::ostream& os, const Event& event);

 private:
  const std::string kMessage_;
  const std::string kName_;
  const EventTypes  kType_;
  const float kValue_;
};

std::ostream& operator<<(std::ostream& os, const Event& event);

} // end namespace kuge

#endif // KUGE_HEADERS_EVENT_H_