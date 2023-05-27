#pragma once

#include <atomic>
#include <memory>
#include <mutex>

#include "dispatch_task.h"
#include "pending_task.h"
#include "task_loop.h"

namespace mk {
class TaskPump;
class TaskQueue;
class TimeProvider;

/**
 * @brief Processes tasks for thread.
 *
 */
class RunLoop : public TaskLoop, public DispatchTask {
 public:
  RunLoop(std::shared_ptr<TaskPump> task_pump,
          std::shared_ptr<TaskQueue> task_queue,
          std::shared_ptr<TimeProvider> time_provider);

  /** @see TaskLoop.*/
  void Run() override;

  /** @see TaskLoop. */
  void Stop() override;

  /** @see DispatchTask. */
  TaskHandle PostTask(Task task) override;

  /** @see DispatchTask. */
  TaskHandle PostRepeatingTask(Task task, size_t times,
                               IntervalMs period) override;

  /** @see DispatchTask. */
  TaskHandle PostDelayedTask(Task task, IntervalMs delay) override;

  /** @see DispatchTask. */
  void CancelTask(TaskHandle&& handle) override;

 private:
  TaskHandle PostTask(Task task, size_t times, IntervalMs period,
                      TimestampMs when);

  void PostTask(std::shared_ptr<PendingTask>&& task);

  std::shared_ptr<TaskPump> pump_;
  std::shared_ptr<TaskQueue> queue_;
  std::shared_ptr<TimeProvider> time_provider_;

  std::mutex task_quard_;
  std::atomic<bool> is_running_;
};
}  // namespace mk