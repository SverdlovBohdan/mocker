#pragma once

#include <stddef.h>

#include <functional>

#include "time_types.h"

namespace mk {
using Task = std::function<void()>;

/**
 * @brief Encapsulates pending task data.
 *
 */
struct PendingTask {
 public:
  PendingTask() : task{[]() {}}, times{0}, period{0}, next_call{0} {};

  PendingTask(Task&& pending_task, size_t count, IntervalMs interval,
              TimestampMs when)
      : task{std::move(pending_task)},
        times{count},
        period{interval},
        next_call{when} {}

  PendingTask(Task&& pending_task, TimestampMs when)
      : PendingTask(std::move(pending_task), 1, IntervalMs{0}, when) {}

  Task task;
  size_t times;
  IntervalMs period;
  TimestampMs next_call;
};
}  // namespace mk