#include "event.hpp"
#include "system.hpp"

void kuge::EventBus::processEvents() {
  const auto q_size {events_.size()};
  for (std::size_t i = 0; i < q_size; ++i) {
    for (const auto& system: systems_) {
      system->handleEvent(*events_.front());
    }
    events_.front().reset(nullptr);
    events_.pop();
  }
}
