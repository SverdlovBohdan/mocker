#pragma once

#include <SDL3/SDL_opengl.h>

#include <cstddef>
#include <memory>
#include <system_error>
#include <optional>
#include <tl/expected.hpp>
#include <vector>

#include "base/task_handle.h"
#include "image_view.h"

namespace mk {
class DispatchTask;

class Image : public ImageView, public std::enable_shared_from_this<Image> {
 public:
  Image(std::filesystem::path image_path,
        std::shared_ptr<DispatchTask> ui_task_dispatcher,
        std::shared_ptr<DispatchTask> filesystem_task_dispatcher);

  ~Image();

  /** @see ImageView. */
  void Display() override;

 private:
  enum class ReadyStatus {
    kNone,
    kReading,
    kError,
    kReady,
  };

  struct ImageTexture {
    std::vector<std::byte> image_data;
    std::size_t width{0};
    std::size_t height{0};
    std::size_t channels{0};
  };

  tl::expected<ImageTexture, std::error_code> LoadImageDataFromFile(
      std::filesystem::path image_path);

  void OnTextureReadingSuccess(ImageTexture image_texture);
  void OnError();

  std::shared_ptr<DispatchTask> ui_task_dispatcher_;
  std::shared_ptr<DispatchTask> filesystem_task_dispatcher_;
  std::filesystem::path image_path_;

  ReadyStatus status_;
  TaskHandle image_reading_task_handle_;
  ImageTexture texture_;

  std::optional<intptr_t> image_texture_id_;
};
}  // namespace mk