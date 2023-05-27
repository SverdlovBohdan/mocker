#include "run_loop.h"

#include <limits>

#include "task_pump.h"
#include "task_queue.h"
#include "time_provider.h"

namespace mk {
RunLoop::RunLoop(std::shared_ptr<TaskPump> task_pump,
                 std::shared_ptr<TaskQueue> task_queue,
                 std::shared_ptr<TimeProvider> time_provider)
    : pump_{std::move(task_pump)},
      queue_{std::move(task_queue)},
      time_provider_{std::move(time_provider)},
      is_running_{false} {}

void RunLoop::Run() {
  is_running_ = true;

  while (is_running_) {
    std::unique_lock lock{task_quard_};

    if (queue_->IsEmpty()) {
      lock.unlock();
      pump_->WaitUntil(TimestampMs::max());
      continue;
    }

    const auto now = time_provider_->Now();
    const auto call_time = queue_->GetNextTaskCallTime();
    const bool is_ready_to_perform = call_time <= now;

    if (is_ready_to_perform) {
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
      }

    } else {
      task_quard_.unlock();
      pump_->WaitUntil(call_time);
    }
  }
}

void RunLoop::Stop() {
  is_running_ = false;
  pump_->Notify();
}

TaskHandle RunLoop::PostTask(Task task) {
  return PostDelayedTask(std::move(task), IntervalMs{0});
}

TaskHandle RunLoop::PostRepeatingTask(Task task, size_t times,
                                      IntervalMs period) {
  return PostTask(std::move(task), times, period, time_provider_->Now());
}

TaskHandle RunLoop::PostDelayedTask(Task task, IntervalMs delay) {
  return PostTask(std::move(task), 1, IntervalMs{0},
                  time_provider_->Now() + delay);
}

void RunLoop::CancelTask(TaskHandle&& handle) {
  auto task_ptr = handle.task.lock();
  if (std::lock_guard lock{task_quard_}; task_ptr) {
    task_ptr->task = []() {};
    task_ptr->times = 0;
  }
}

TaskHandle RunLoop::PostTask(Task task, size_t times, IntervalMs period,
                             TimestampMs when) {
  TaskHandle handle;
  {
    std::unique_lock lock{task_quard_};
    handle = queue_->AddTask(
        std::make_shared<PendingTask>(std::move(task), times, period, when));
  }
  pump_->Notify();

  return handle;
}

void RunLoop::PostTask(std::shared_ptr<PendingTask>&& task) {
  {
    std::unique_lock lock{task_quard_};
    queue_->AddTask(std::move(task));
  }

  pump_->Notify();
}

}  // namespace mk