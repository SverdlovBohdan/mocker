#pragma once

#include <memory>

#include "pending_task.h"
#include "task_handle.h"

namespace mk {
/**
 * @brief Task queue interface.
 *
 */
class TaskQueue {
 public:
  virtual ~TaskQueue() = default;

  /**
   * @brief Add task to queue.
   *
   * @param task Task to be added.
   */
  virtual TaskHandle AddTask(std::shared_ptr<PendingTask> task) = 0;

  /**
   * @brief Pop task from queue.
   *
   * @return Task or nullptr.
   */
  virtual std::shared_ptr<PendingTask> PopTask() = 0;

  /**
   * @brief Tell if queue is empty.
   *
   * @return true if queue is empty. Otherwise false.
   */
  virtual bool IsEmpty() const = 0;

  /**
   * @brief Get the Next Task Call timestamp.
   *
   * @return Task call timestamp.
   */
  virtual TimestampMs GetNextTaskCallTime() const = 0;
};
}  // namespace mk