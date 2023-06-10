#pragma once

#include <SDL3/SDL_opengl.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <system_error>
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
  /**
   * @brief Image ready status.
   *
   */
  enum class ReadyStatus {
    kNone,     ///< Initialialization state.
    kReading,  ///< Image data is reading from file.
    kError,    ///< Image reading has failed.
    kReady,    ///< Image is ready to display.
  };

  /**
   * @brief Image texture context.
   *
   */
  struct ImageTexture {
    std::vector<std::byte> image_data;
    std::size_t width{0};
    std::size_t height{0};
    std::size_t channels{0};
  };

  /**
   * @brief Load image data from file on filesystem thread.
   *
   * @return TaskHandle Filesystem thread task handle.
   */
  TaskHandle LoadImageFromFileOnFilesystemThread();

  /**
   * @brief Load image data from file.
   *
   * @param image_path Absoluth path to file.
   * @return ImageTexture in success. Otherwise error code.
   */
  tl::expected<ImageTexture, std::error_code> LoadImageDataFromFile(
      std::filesystem::path image_path);

  /**
   * @brief Generate texture and push image data to GPU.
   *
   * @return Texture id.
   */
  intptr_t GenerateImageOpenGlTexture();

  // Handlers in UI thread.
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