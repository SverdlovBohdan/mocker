#pragma once

#include <memory>

#include "pending_task.h"

namespace mk {
struct TaskHandle {
  TaskHandle() : TaskHandle{nullptr} {}

  explicit TaskHandle(const std::shared_ptr<PendingTask>& pending_task)
      : task{pending_task} {}

  operator bool() const { return static_cast<bool>(task.lock()); }

  std::weak_ptr<PendingTask> task;
};
}  // namespace mk