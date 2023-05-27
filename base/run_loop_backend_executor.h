#pragma once

#include <functional>

namespace mk {
/**
 * @brief Execute backend specific task (Example: Draw UI in UI run loop, etc).
 *
 */
class RunLoopBackendExecutor {
 public:
  enum class IterationStatus {
    Ok,    ///< Normal iteration finish.
    Done,  ///< Backend execution has stopped.
  };

  using BackendTask = std::function<IterationStatus()>;

  virtual ~RunLoopBackendExecutor() = default;

  /**
   * @brief Set the Backend Task.
   *
   * @param backend_task Backend task.
   */
  virtual void SetBackendTask(BackendTask&& backend_task) = 0;
};
}  // namespace mk