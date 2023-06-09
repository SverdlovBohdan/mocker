#pragma once

#include <filesystem>

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
};
}  // namespace mk