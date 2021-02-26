#ifndef KUGE_HEADERS_OUTPUT_SYSTEM_H_
#define KUGE_HEADERS_OUTPUT_SYSTEM_H_

#include "event.h"
#include "system.h"
#include <iostream>

namespace kuge {

class OutputSystem : virtual public System {
 public:
  OutputSystem(EventBus& bus): System(bus) { /* std::cout.sync_with_stdio(false); */ }
  void handleEvent(const Event& event) { std::cout << event; }
};

} // end namespace kuge

#endif // KUGE_HEADERS_OUTPUT_SYSTEM_H_