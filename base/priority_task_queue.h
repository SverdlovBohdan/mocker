#pragma once

#include <functional>
#include <queue>

#include "task_queue.h"

namespace mk {
/**
 * @brief Task call time priority queue.
 *
 */
class PriorityTaskQueue : public TaskQueue {
 public:
  /** @see TaskQueue. */
  TaskHandle AddTask(std::shared_ptr<PendingTask> task) override;

  /** @see TaskQueue. */
  std::shared_ptr<PendingTask> PopTask() override;

  /** @see TaskQueue. */
  bool IsEmpty() const override;

  /** @see TaskQueue. */
  TimestampMs GetNextTaskCallTime() const override;

 private:
  /**
   * @brief Pending task priority comparator.
   *
   */
  struct PendignTaskPriorityComparator {
    bool operator()(std::shared_ptr<PendingTask> lhs,
                    std::shared_ptr<PendingTask> rhs) const {
      return lhs->next_call > rhs->next_call;
    }
  };

  std::priority_queue<std::shared_ptr<PendingTask>,
                      std::vector<std::shared_ptr<PendingTask>>,
                      PendignTaskPriorityComparator>
      queue_;
};
}  // namespace mk