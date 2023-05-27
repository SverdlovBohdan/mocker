#pragma once

namespace mk {
/**
 * @brief Main application interface.
 *
 */
class UiApplication {
 public:
  enum class Status {
    Ok,
    Error
  };

  virtual ~UiApplication() = default;

  /**
   * @brief Run application.
   *
   */
  virtual Status Run() = 0;
};
}  // namespace mk