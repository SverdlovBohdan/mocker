#pragma once

#include "filesystem_reader.h"

namespace mk {
class FilesystemBrowser : public FilesystemReader {
 public:
  FilesystemBrowser() = default;

  /** @see FilesystemBrowser. */
  Directory OpenDirectory(std::optional<std::filesystem::path> path) override;
};
}  // namespace mk