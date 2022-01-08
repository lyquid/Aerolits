#ifndef KUGE_HEADERS_BACKEND_SYSTEM_HPP_
#define KUGE_HEADERS_BACKEND_SYSTEM_HPP_

#include "event.hpp"
#include "system.hpp"

namespace kuge {

class BackendSystem: public System {
 public:
  virtual void handleEvent(const KugeEvent* event) override {}
 private:
};

} // namespace kuge

#endif // KUGE_HEADERS_BACKEND_SYSTEM_HPP_
