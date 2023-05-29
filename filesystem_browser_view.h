#pragma once

#include <filesystem>
#include <functional>
#include <vector>
#include <string_view>

namespace mk {
class FilesystemBrowserView {
 public:
  using SelectedFilesHandler =
      std::function<void(std::vector<std::filesystem::path>)>;

  virtual ~FilesystemBrowserView() = default;

  /**
   * @brief Display filesystem browser.
   *
   */
  virtual void Display(std::string_view id) = 0;

  /**
   * @brief Set the Selected Files Handler object
   *
   * @param handler
   */
  virtual void SetSelectedFilesHandler(SelectedFilesHandler handler) = 0;
};
}  // namespace mk