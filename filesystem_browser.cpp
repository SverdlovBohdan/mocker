#include "filesystem_browser.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string_view>

#include "imgui.h"

namespace mk {
FilesystemBrowser::FilesystemBrowser()
    : selected_files_handler_{nullptr},
      is_directory_changed_{true},
      next_directory_{FilesystemReader::CurrentDirectory} {}

Directory FilesystemBrowser::OpenDirectory(
    std::optional<std::filesystem::path> path) {
  if (!path || (path && !std::filesystem::exists(*path))) {
    path = std::filesystem::current_path();
  }

  Directory openedDirectory{*path};
  openedDirectory.ReadContent();
  return openedDirectory;
}

std::vector<std::filesystem::path> FilesystemBrowser::ExtractDirectories(
    const std::vector<std::filesystem::path>& directory_content) {
  std::vector<std::filesystem::path> directories{};
  directories.reserve(directory_content.size());

  for (const auto& item : directory_content) {
    if (std::filesystem::is_directory(item)) {
      directories.emplace_back(item);
    }
  }

  return directories;
}

std::vector<std::filesystem::path> FilesystemBrowser::ExtractFiles(
    const std::vector<std::filesystem::path>& directory_content) {
  std::vector<std::filesystem::path> files{};
  files.reserve(directory_content.size());

  for (const auto& item : directory_content) {
    if (!std::filesystem::is_directory(item)) {
      files.emplace_back(item);
    }
  }

  return files;
}

void FilesystemBrowser::Display(std::string_view id) {
  // Always center this window when appearing
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal(id.data())) {
    if (is_directory_changed_) {
      observed_directory_.emplace(OpenDirectory(next_directory_));
      is_directory_changed_ = false;
      selected_files_state_ = std::vector<bool>(
          ExtractFiles(observed_directory_->List()).size(), false);
    }

    if (ImGui::SmallButton("..")) {
      assert(observed_directory_ &&
             "observed_directory_ must exist if you push |..|");

      is_directory_changed_ = true;
      next_directory_ = observed_directory_->GetParentDirectory();
    }

    const auto directories = ExtractDirectories(observed_directory_->List());
    for (const auto& directory_path : directories) {
      if (ImGui::SmallButton(directory_path.c_str())) {
        is_directory_changed_ = true;
        next_directory_ = directory_path;
      }
    }

    const auto selectable_files = ExtractFiles(observed_directory_->List());
    for (size_t n = 0; n < selectable_files.size(); ++n) {
      if (ImGui::Selectable(selectable_files[n].c_str(),
                            selected_files_state_[n],
                            ImGuiSelectableFlags_DontClosePopups)) {
        if (!ImGui::GetIO().KeyCtrl) {
          selected_files_state_ =
              std::vector<bool>(selectable_files.size(), false);
        }
        selected_files_state_[n] = !selected_files_state_[n];
      }
    }

    if (ImGui::Button("Select", ImVec2(120, 0))) {
      if (selected_files_handler_) {
        selected_files_handler_({});
      }
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}

void FilesystemBrowser::SetSelectedFilesHandler(SelectedFilesHandler handler) {
  selected_files_handler_ = std::move(handler);
}
}  // namespace mk