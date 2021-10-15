#ifndef KUGE_HEADERS_EVENT_HPP_
#define KUGE_HEADERS_EVENT_HPP_

#include <SDL.h>
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace kuge {

class KugeEvent;
class System;

class EventBus {

 public:

  /**
   * @brief Post a KugeEvent into the events queue.
   * @tparam T a KugeEvent derived class.
   * @param event The event to be put in queue.
   */
  template<typename T>
  inline void postEvent(const T& event) {
    events_.push(std::make_unique<T>(event));
  }

  /**
   * @brief NOT TESTED!
   * @tparam T
   * @param event
   */
  template<typename T>
  void postImmediateEvent(const T& event) const {
    for (const auto& system: systems_) system->handleEvent(event);
  }

  /**
   * @brief Broadcasts every KugeEvent in the queue to all the Systems.
   */
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

  std::queue<std::unique_ptr<KugeEvent>> events_ {};
  std::vector<std::unique_ptr<System>> systems_ {};
};

/* KUGE EVENTS */

enum class KugeEventTypes {
  AeroliteDestroyed,
  EnterDemoState,
  PlayerDead
};

class KugeEvent {
 public:
  virtual ~KugeEvent() {}
  inline auto name() const { return name_; }
  inline auto type() const { return type_; }
 protected:
  KugeEvent(KugeEventTypes type, const std::string& name): name_(name), type_(type) {}
  const std::string name_;
  const KugeEventTypes type_;
};

class AeroliteDestroyedEvent: public KugeEvent {
 public:
  AeroliteDestroyedEvent(KugeEventTypes type): KugeEvent(type, "Aerolite destroyed") {}
  inline auto position() const { return position_; }
 private:
  SDL_FPoint position_ {};
};

class EnterDemoStateEvent: public KugeEvent {
 public:
  EnterDemoStateEvent(KugeEventTypes type): KugeEvent(type, "Enter demo state") {}
 private:
};

} // end namespace kuge

#endif // KUGE_HEADERS_EVENT_HPP_