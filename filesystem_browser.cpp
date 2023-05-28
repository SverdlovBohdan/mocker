#include "filesystem_browser.h"

#include <filesystem>

namespace mk {
Directory FilesystemBrowser::OpenDirectory(
    std::optional<std::filesystem::path> path) {
  if (!path || (path && !std::filesystem::exists(*path))) {
    path = std::filesystem::current_path();
  }

  Directory openedDirectory{*path};
  openedDirectory.ReadContent();
  return openedDirectory;
}
}  // namespace mk