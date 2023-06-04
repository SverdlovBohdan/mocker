#pragma once

#include <atomic>
#include <memory>
#include <mutex>

#include "dispatch_task.h"
#include "pending_task.h"
#include "run_loop_backend_executor.h"
#include "task_loop.h"

namespace mk {
class TaskQueue;
class TimeProvider;

/**
 * @brief Processes tasks for ui thread.
 *
 */
class RunLoopUi : public TaskLoop,
                  public DispatchTask,
                  public RunLoopBackendExecutor {
 public:
  RunLoopUi(std::unique_ptr<TaskQueue> task_queue,
            std::unique_ptr<TimeProvider> time_provider);

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

  /** @see RunLoopBackendExecutor. */
  void SetBackendTask(BackendTask&& backend_task) override;

 private:
  TaskHandle PostTask(Task task, size_t times, IntervalMs period,
                      TimestampMs when);

  void PostTask(std::shared_ptr<PendingTask>&& task);

  std::unique_ptr<TaskQueue> queue_;
  std::unique_ptr<TimeProvider> time_provider_;

  std::mutex task_quard_;
  std::atomic<bool> is_running_;

  RunLoopBackendExecutor::BackendTask backend_task_;
};
}  // namespace mk