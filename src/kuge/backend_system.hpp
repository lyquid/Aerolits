#ifndef KUGE_HEADERS_BACKEND_SYSTEM_HPP_
#define KUGE_HEADERS_BACKEND_SYSTEM_HPP_

#include "event.hpp"
#include "system.hpp"
#include <limits>

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

  BackendSystem() { frame_times_.resize(kFrameBufferSize_); }
  virtual void handleEvent(const KugeEvent* event) override {}
  void draw();
  void resetStatistics();

 private:

  void mainMenuBar();
  void statisticsWindow(bool* show);

  B2DebugFlags debug_flags_ {};
  bool statistics_ {true};

  static constexpr auto kFrameBufferSize_ {100u};
  static constexpr auto kPollInterval_ {100u}; // ms
  std::size_t        frame_index_ {};
  std::vector<float> frame_times_ {};
  int frame_accumulator_ {};
  int frame_accumulator_count_ {};
  unsigned int time_last_added_ {};
  double max_frame_time_ {};
  double min_frame_time_ {std::numeric_limits<double>::max()};
};

} // namespace kuge

#endif // KUGE_HEADERS_BACKEND_SYSTEM_HPP_
