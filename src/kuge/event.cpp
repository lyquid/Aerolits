#include "event.hpp"
#include "system.hpp"

void kuge::EventBus::processEvents() {
  const auto q_size {events_.size()};
  for (std::size_t i = 0; i < q_size; ++i) {
    for (const auto& system: systems_) {
      system->handleEvent(events_.front().get());
    }
    events_.front().reset(nullptr);
    events_.pop();
  }
}

/* AeroliteDestroyedEvent */

void kuge::AeroliteDestroyedEvent::print() const {
  ktp::logInfo(
    '[' + std::to_string(timestamp_) + "ms] "
    + name_ + " (" + std::to_string(score_) + "pts) at "
    + std::to_string((int)position_.x) + ", "
    + std::to_string((int)position_.y)
  );
}

/* AeroliteSplittedEvent */

void kuge::AeroliteSplittedEvent::print() const {
  ktp::logInfo(
    '[' + std::to_string(timestamp_) + "ms] "
    + name_ + " in " + std::to_string(pieces_ + 1)
    + " (" + std::to_string(score_) + "pts) at "
    + std::to_string((int)position_.x) + ", "
    + std::to_string((int)position_.y)
  );
}