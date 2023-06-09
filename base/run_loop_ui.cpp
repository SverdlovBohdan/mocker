#include "run_loop_ui.h"

#include <limits>

#include "task_queue.h"
#include "time_provider.h"

namespace mk {
RunLoopUi::RunLoopUi(std::unique_ptr<TaskQueue> task_queue,
                     std::unique_ptr<TimeProvider> time_provider)
    : queue_{std::move(task_queue)},
      time_provider_{std::move(time_provider)},
      is_running_{false},
      backend_task_{nullptr} {}

void RunLoopUi::Run() {
  is_running_ = true;

  while (is_running_) {
    std::unique_lock lock{task_quard_};

    if (!queue_->IsEmpty()) {
      const auto now = time_provider_->Now();
      bool is_ready_to_perform = queue_->GetNextTaskCallTime() <= now;

      while (is_ready_to_perform) {
        auto pending_task = queue_->PopTask();
        auto task = pending_task->task;

        lock.unlock();
        task();
        lock.lock();

        if (!is_running_) {
          break;
        }

        if (pending_task->times > 1) {
          --pending_task->times;
          pending_task->next_call = now + pending_task->period;

          task_quard_.unlock();
          PostTask(std::move(pending_task));
          task_quard_.lock();
        }

        is_ready_to_perform =
            queue_->IsEmpty() ? false : queue_->GetNextTaskCallTime() <= now;
      }
    }

    lock.unlock();
    if (backend_task_) {
      is_running_ =
          backend_task_() == RunLoopBackendExecutor::IterationStatus::Ok;
    }
  }
}

void RunLoopUi::Stop() { is_running_ = false; }

TaskHandle RunLoopUi::PostTask(Task task) {
  return PostDelayedTask(std::move(task), IntervalMs{0});
}

TaskHandle RunLoopUi::PostRepeatingTask(Task task, size_t times,
                                        IntervalMs period) {
  return PostTask(std::move(task), times, period, time_provider_->Now());
}

TaskHandle RunLoopUi::PostDelayedTask(Task task, IntervalMs delay) {
  return PostTask(std::move(task), 1, IntervalMs{0},
                  time_provider_->Now() + delay);
}

void RunLoopUi::CancelTask(TaskHandle&& handle) {
  auto task_ptr = handle.task.lock();
  if (std::lock_guard lock{task_quard_}; task_ptr) {
    task_ptr->task = []() {};
    task_ptr->times = 0;
  }
}

TaskHandle RunLoopUi::PostTask(Task task, size_t times, IntervalMs period,
                               TimestampMs when) {
  TaskHandle handle;
  {
    std::unique_lock lock{task_quard_};
    handle = queue_->AddTask(
        std::make_shared<PendingTask>(std::move(task), times, period, when));
  }

  return handle;
}

void RunLoopUi::PostTask(std::shared_ptr<PendingTask>&& task) {
  {
    std::unique_lock lock{task_quard_};
    queue_->AddTask(std::move(task));
  }
}

void RunLoopUi::SetBackendTask(BackendTask&& backend_task) {
  backend_task_ = std::move(backend_task);
}
}  // namespace mk