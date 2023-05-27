#pragma once

#include <condition_variable>

#include "task_pump.h"
#include "dispatch_task.h"

namespace mk {
/**
 * @brief Controls message pumping by c++ std.
 *
 */
class TaskPumpStd : public TaskPump {
 public:
  /** @see TaskPump. */
  void Run(std::shared_ptr<DispatchTask> task_dispatcher) override;

  /** @see TaskPump. */
  void WaitUntil(TimestampMs time) override;

  /** @see TaskPump. */
  void Notify() override;

 protected:  // for tests
  virtual void Wait(std::unique_lock<std::mutex>& lock);
  virtual void WaitUntilTime(std::unique_lock<std::mutex>& lock, TimestampMs time);
  virtual void NotifyAll();

 private:
  std::condition_variable event_;
  std::mutex guard_;
};
}  // namespace mk