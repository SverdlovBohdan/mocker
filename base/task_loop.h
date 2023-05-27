#pragma once

namespace mk {
/**
 * @brief Message loop interface.
 *
 */
class TaskLoop {
 public:
    virtual ~TaskLoop () = default;

 /**
  * @brief Run message loop processing.
  *
  */
  virtual void Run() = 0;

  /**
   * @brief Stops message loop.
   *
   */
  virtual void Stop() = 0;
};
}  // namespace mk