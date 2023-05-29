#pragma once

#include <vector>
#include <optional>

#include "filesystem_browser_view.h"
#include "filesystem_reader.h"

namespace mk {
class FilesystemBrowser : public FilesystemReader,
                          public FilesystemBrowserView {
 public:
  FilesystemBrowser();

  /** @see FilesystemBrowser. */
  Directory OpenDirectory(std::optional<std::filesystem::path> path) override;

  /** @see FilesystemBrowserView. */
  void Display(std::string_view id) override;

  /** @see FilesystemBrowserView. */
  void SetSelectedFilesHandler(SelectedFilesHandler handler) override;

 private:
  SelectedFilesHandler selected_files_handler_;
  bool is_directory_changed_;
  std::optional<Directory> observed_directory_;
  std::vector<bool> selected_files_state_;
};
}  // namespace mk