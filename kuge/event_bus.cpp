#include "event_bus.h"
#include "input_system.h"
#include "output_system.h"

void kuge::EventBus::postImmediateEvent(const Event& event) const {
  for (const auto& system: systems_) {
    system->handleEvent(event);
  }
}

void kuge::EventBus::postImmediateEvent(EventTypes event_t) const {
  const kuge::Event event(event_t);
  for (const auto& system: systems_) {
    system->handleEvent(event);
  }
}

void kuge::EventBus::postImmediateEvent(EventTypes event_t, float value) const {
  const kuge::Event event(event_t, value);
  for (const auto& system: systems_) {
    system->handleEvent(event);
  }
}

void kuge::EventBus::postImmediateEvent(EventTypes event_t, const std::string& msg) const {
  const kuge::Event event(event_t, msg);
  for (const auto& system: systems_) {
    system->handleEvent(event);
  }
}

void kuge::EventBus::postImmediateEvent(EventTypes event_t, float value, const std::string& msg) const {
  const kuge::Event event(event_t, value, msg);
  for (const auto& system: systems_) {
    system->handleEvent(event);
  }
}

void kuge::EventBus::processEvents() {
  const auto q_size = events_.size();
  for (auto i = 0u; i < q_size; ++i) {
    for (const auto& system: systems_) {
      system->handleEvent(events_.front());
    }
    events_.pop();
  }
}
