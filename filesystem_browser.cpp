#include "filesystem_browser.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string_view>

#include "imgui.h"

namespace mk {
FilesystemBrowser::FilesystemBrowser()
    : selected_files_handler_{nullptr}, is_directory_changed_{true} {}

Directory FilesystemBrowser::OpenDirectory(
    std::optional<std::filesystem::path> path) {
  if (!path || (path && !std::filesystem::exists(*path))) {
    path = std::filesystem::current_path();
  }

  Directory openedDirectory{*path};
  openedDirectory.ReadContent();
  return openedDirectory;
}

void FilesystemBrowser::Display(std::string_view id) {
  // Always center this window when appearing
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal(id.data())) {
    if (is_directory_changed_) {
      observed_directory_.emplace(OpenDirectory(
          observed_directory_ ? observed_directory_->GetParentDirectory()
                              : std::optional<std::filesystem::path>(
                                    FilesystemReader::CurrentDirectory)));
      is_directory_changed_ = false;
      selected_files_state_ =
          std::vector<bool>(observed_directory_->List().size(), false);
    }

    const auto& directory_content = observed_directory_->List();
    for (size_t n = 0; n < directory_content.size(); ++n) {
      if (ImGui::Selectable(directory_content[n].c_str(),
                            selected_files_state_[n],
                            ImGuiSelectableFlags_DontClosePopups)) {
        if (!ImGui::GetIO().KeyCtrl) {
          selected_files_state_ =
              std::vector<bool>(directory_content.size(), false);
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