#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace mk {
class Directory {
 public:
  explicit Directory(std::filesystem::path path) : path_{std::move(path)} {}

  std::filesystem::path GetParentDirectory() { return path_.parent_path(); };

  const std::vector<std::filesystem::path>& List() const { return content_; }

  void ReadContent() {
    for (auto const& item : std::filesystem::directory_iterator{path_}) {
      content_.emplace_back(item.path());
    }
  }

 private:
  const std::filesystem::path path_;
  std::vector<std::filesystem::path> content_;
};

class FilesystemReader {
 public:
  static constexpr std::nullopt_t CurrentDirectory = std::nullopt;

  virtual ~FilesystemReader() = default;

  /**
   * @brief Read directory content.
   *
   * @param path Path to the directory. If std::nullopt reads current process
   * directory.
   * @return Directory
   */
  virtual Directory OpenDirectory(
      std::optional<std::filesystem::path> path) = 0;

  /**
   * @brief Extract directories paths from path list.
   *
   * @param directory_content Directory items list.
   * @return List of directory paths.
   */
  virtual std::vector<std::filesystem::path> ExtractDirectories(
      const std::vector<std::filesystem::path>& directory_content) = 0;

  /**
   * @brief Extract files paths from path list.
   *
   * @param directory_content Directory items list.
   * @return List of file paths.
   */
  virtual std::vector<std::filesystem::path> ExtractFiles(
      const std::vector<std::filesystem::path>& directory_content) = 0;
};
}  // namespace mk