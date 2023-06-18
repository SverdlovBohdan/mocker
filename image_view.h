#pragma once

#include <filesystem>
#include <functional>

namespace mk {
class ImageView {
 public:
  virtual ~ImageView() = default;

  /**
   * @brief Display image from path.
   *
   * Call expected from UI thread.
   *
   */
  virtual void Display() = 0;

  /**
   * @brief Set the image size.
   *
   * @param width Image width.
   * @param height Image height.
   */
  virtual void SetSize(std::size_t width, std::size_t height) = 0;

/**
 * @brief Set the Error Handler.
 *
 * @param handler To be called in error case.
 */
  virtual void SetErrorHandler(
      std::function<void(const std::filesystem::path&)> handler) = 0;

/**
 * @brief Set the Progress Handler.
 *
 * @param handler Called during image reading.
 */
  virtual void SetProgressHandler(std::function<void()> handler) = 0;
};
}  // namespace mk