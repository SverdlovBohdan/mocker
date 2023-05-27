#include "steady_time_provider.h"

namespace mk {
TimestampMs SteadyTimeProvider::Now() const {
  return std::chrono::duration_cast<TimestampMs>(
      std::chrono::steady_clock::now().time_since_epoch());
}
}  // namespace mk