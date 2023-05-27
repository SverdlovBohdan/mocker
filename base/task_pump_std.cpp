#include "task_pump_std.h"

namespace mk {
void TaskPumpStd::Run(std::shared_ptr<DispatchTask>) {}

void TaskPumpStd::WaitUntil(TimestampMs time) {
  if (std::unique_lock lock{guard_}; time == TimestampMs::max()) {
    Wait(lock);
  } else {
    WaitUntilTime(lock, time);
  }
}

void TaskPumpStd::Notify() { NotifyAll(); }

void TaskPumpStd::Wait(std::unique_lock<std::mutex>& lock) {
  event_.wait(lock);
}

void TaskPumpStd::WaitUntilTime(std::unique_lock<std::mutex>& lock,
                            TimestampMs time) {
  event_.wait_until(lock, std::chrono::steady_clock::time_point{time});
}

void TaskPumpStd::NotifyAll() { event_.notify_all(); }
}  // namespace mk