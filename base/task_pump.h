#pragma once

#include <memory>

#include "time_types.h"

namespace mk {
class DispatchTask;

/**
 * @brief Controls task pumping.
 *
 */
class TaskPump {
 public:
  virtual ~TaskPump() = default;

  /**
   * @brief Run task pumping.
   *
   * @param task_dispatcher Task dispatcher.
   */
  virtual void Run(std::shared_ptr<DispatchTask> task_dispatcher) = 0;

  /**
   * @brief Wait until.
   *
   * @param time Wait for time.
   */
  virtual void WaitUntil(TimestampMs time) = 0;

  /**
   * @brief Notify changes.
   *
   */
  virtual void Notify() = 0;
};
}  // namespace mk