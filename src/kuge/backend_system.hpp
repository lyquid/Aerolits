#ifndef KUGE_HEADERS_BACKEND_SYSTEM_HPP_
#define KUGE_HEADERS_BACKEND_SYSTEM_HPP_

#include "event.hpp"
#include "system.hpp"

namespace kuge {

struct B2DebugFlags {
  bool aabb   {false};
  bool center {false};
  bool joints {false};
  bool pairs  {false};
  bool shapes {false};
};

class BackendSystem: public System {
 public:
  virtual void handleEvent(const KugeEvent* event) override {}
  void draw();
 private:
  void mainMenuBar();
  void statisticsWindow(bool* show);

  B2DebugFlags debug_flags_ {};
  bool statistics_ {true};
};

} // namespace kuge

#endif // KUGE_HEADERS_BACKEND_SYSTEM_HPP_
