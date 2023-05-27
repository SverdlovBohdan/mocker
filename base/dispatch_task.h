#pragma once

#include "task_handle.h"

namespace mk {
/**
 * @brief Task dispatcher interface.
 *
 */
class DispatchTask {
 public:
  virtual ~DispatchTask() = default;

  /**
   * @brief Post immeadetly task.
   *
   * @param task Task to be done.
   * @return Task handle.
   */
  virtual TaskHandle PostTask(Task task) = 0;

  /**
   * @brief Post repeating task.
   *
   * @param task Task to be done.
   * @param times Task execution times.
   * @param period Delay between tasks execution.
   * @return Task handle.
   */
  virtual TaskHandle PostRepeatingTask(Task task, size_t times,
                                       IntervalMs period) = 0;

  /**
   * @brief Post delayed task.
   *
   * @param task Task to be done.
   * @param delay Delay before task execution.
   * @return Task handle.
   */
  virtual TaskHandle PostDelayedTask(Task task, IntervalMs delay) = 0;

  /**
   * @brief Try to cancle task by handle.
   *
   * @param handle Task handle.
   */
  virtual void CancelTask(TaskHandle&& handle) = 0;
};
}  // namespace mk