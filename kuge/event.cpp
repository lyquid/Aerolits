#include "event.h"

kuge::Event::Event(kuge::EventTypes type):
  kMessage_(std::string()),
  kName_(kuge::kEventTypesNames.at(type)),
  kType_(type), 
  kValue_(NAN) {}

kuge::Event::Event(kuge::EventTypes type, float value):
  kMessage_(std::string()),
  kName_(kuge::kEventTypesNames.at(type)),
  kType_(type), 
  kValue_(value) {}

kuge::Event::Event(kuge::EventTypes type, const std::string& msg):
  kMessage_(msg),
  kName_(kuge::kEventTypesNames.at(type)),
  kType_(type),
  kValue_(NAN) {}

kuge::Event::Event(kuge::EventTypes type, float value, const std::string& msg):
  kMessage_(msg),
  kName_(kuge::kEventTypesNames.at(type)),
  kType_(type),
  kValue_(value) {}

std::ostream& kuge::operator<<(std::ostream& os, const kuge::Event& event) {
  os << '['
     << static_cast<std::underlying_type<kuge::EventTypes>::type>(event.kType_)
     << "]\t"
     << event.kName_
     << '(';

  if (!std::isnan(event.kValue_)) {           // value 
    os << event.kValue_;
    if (!event.kMessage_.empty()) {           // both
      os << ", \"" << event.kMessage_ << "\"";
    } 
  } else if (!event.kMessage_.empty()) {      // msg
    os << "\"" << event.kMessage_ << "\"";;
  }

  os << ")\n";
  return os;
}