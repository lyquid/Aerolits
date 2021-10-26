#ifndef KUGE_HEADERS_EVENT_HPP_
#define KUGE_HEADERS_EVENT_HPP_

#include "../sdl2_wrappers/sdl2_log.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
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
  void setSystems(T* t) {
    systems_.push_back(t);
  }

  template<typename T, typename... Args>
  void setSystems(T* t, Args... args) {
    systems_.push_back(t);
    setSystems(args...) ;
  }

 private:

  std::queue<std::unique_ptr<KugeEvent>> events_ {};
  std::vector<System*> systems_ {};
};

/* KUGE EVENTS */

enum class KugeEventTypes {
  AeroliteDestroyed,
  AeroliteSplitted,
  EnterDemoState,
  PlayerDead
};

class KugeEvent {
 public:
  virtual ~KugeEvent() {}
  inline auto name() const { return name_; }
  virtual void print() const = 0;
  inline auto timestamp() const { return timestamp_; }
  inline auto type() const { return type_; }
 protected:
  KugeEvent(KugeEventTypes type, const std::string& name): name_(name), type_(type) {}
  static const ktp::SDL2_Timer& gameplay_timer_;
  const std::string    name_;
  const uint32_t       timestamp_ {gameplay_timer_.milliseconds()};
  const KugeEventTypes type_;
};

class AeroliteDestroyedEvent: public KugeEvent {
 public:
  AeroliteDestroyedEvent(KugeEventTypes type, SDL_FPoint pos):
    KugeEvent(type, "Aerolite destroyed"), position_(pos) {}

  virtual void print() const override;
  inline auto position() const { return position_; }
 private:
  const SDL_FPoint position_;
};

class AeroliteSplittedEvent: public KugeEvent {
 public:
  AeroliteSplittedEvent(KugeEventTypes type, SDL_FPoint pos, Uint32 pieces):
    KugeEvent(type, "Aerolite splitted"), pieces_(pieces), position_(pos) {}

  inline auto pieces() const { return pieces_; }
  virtual void print() const override;
  inline auto position() const { return position_; }
 private:
  const Uint32 pieces_;
  const SDL_FPoint position_;
};

class EnterDemoStateEvent: public KugeEvent {
 public:
  EnterDemoStateEvent(KugeEventTypes type): KugeEvent(type, "Enter demo state") {}
  virtual inline void print() const override { ktp::logInfo(std::to_string(timestamp_) + "ms. " + name_); }
 private:
};

} // end namespace kuge

#endif // KUGE_HEADERS_EVENT_HPP_