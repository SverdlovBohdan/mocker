#include "image.h"

#include <imgui.h>
#include <stb_image.h>

#include <cassert>
#include <iostream>

#include "base/dispatch_task.h"

namespace mk {

Image::Image(std::filesystem::path image_path,
             std::shared_ptr<DispatchTask> ui_task_dispatcher,
             std::shared_ptr<DispatchTask> filesystem_task_dispatcher)
    : ui_task_dispatcher_{std::move(ui_task_dispatcher)},
      filesystem_task_dispatcher_{std::move(filesystem_task_dispatcher)},
      image_path_{std::move(image_path)},
      status_{ReadyStatus::kNone} {}

Image::~Image() {
  if (image_reading_task_handle_) {
    filesystem_task_dispatcher_->CancelTask(
        std::move(image_reading_task_handle_));
    image_reading_task_handle_ = TaskHandle{nullptr};
  }
}

void Image::Display() {
  switch (status_) {
    case ReadyStatus::kError:
      std::cout << "Can't display " << image_path_.c_str() << std::endl;
      break;

    case ReadyStatus::kReading:
      ImGui::Text("Loading %c",
                  "|/-\\"[static_cast<int>(ImGui::GetTime() / 0.05f) & 3]);
      break;

    case ReadyStatus::kNone:
      status_ = ReadyStatus::kReading;
      image_reading_task_handle_ = filesystem_task_dispatcher_->PostTask(
          [lifetime_controller = shared_from_this()]() {
            // Filesystem thread.
            auto texture = lifetime_controller->LoadImageDataFromFile(
                lifetime_controller->image_path_);

            if (texture.has_value()) {
              lifetime_controller->ui_task_dispatcher_->PostTask(
                  [texture_image = std::move(texture.value()),
                   lifetime_controller]() {
                    // UI thread.
                    lifetime_controller->OnTextureReadingSuccess(
                        std::move(texture_image));
                  });
            } else {
              lifetime_controller->ui_task_dispatcher_->PostTask(
                  [lifetime_controller]() {
                    // UI thread.
                    lifetime_controller->OnError();
                  });
            }
          });
      break;

    case ReadyStatus::kReady:
      if (!image_texture_id_) {
        GLenum format = GL_RGB;
        // TODO(BoSv): add more formats.
        if (texture_.channels == 4) {
          format = GL_RGBA;
        }
        // Create a OpenGL texture identifier
        intptr_t image_texture;
        glGenTextures(1, reinterpret_cast<GLuint*>(&image_texture));
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);  // This is required on WebGL for non
                                            // power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);  // Same

        // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                     texture_.width, texture_.height, 0, format,
                     GL_UNSIGNED_BYTE, texture_.image_data.data());
        image_texture_id_ = image_texture;
      }

      if (image_texture_id_) {
        std::cout << "Draw image" << std::endl;
        ImGui::Image(reinterpret_cast<void*>(*image_texture_id_),
                     ImVec2(texture_.width, texture_.height));
      }
      break;

    default:
      break;
  }
}

tl::expected<Image::ImageTexture, std::error_code> Image::LoadImageDataFromFile(
    std::filesystem::path image_path) {
  int x = 0;
  int y = 0;
  int channels = 0;
  unsigned char* image_data =
      stbi_load(image_path.c_str(), &x, &y, &channels, 0);

  if (image_data == nullptr) {
    fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
    return tl::unexpected{std::make_error_code(std::errc::io_error)};
  }

  const auto size = static_cast<std::size_t>(x * y * channels);
  std::vector<std::byte> data{size};
  std::transform(image_data, image_data + size, data.begin(),
                 [](auto item) { return static_cast<std::byte>(item); });

  ImageTexture texture{std::move(data), static_cast<size_t>(x),
                       static_cast<size_t>(y), static_cast<size_t>(channels)};
  stbi_image_free(image_data);

  return texture;
}

void Image::OnTextureReadingSuccess(ImageTexture image_texture) {
  texture_ = std::move(image_texture);
  status_ = ReadyStatus::kReady;
}

void Image::OnError() { status_ = ReadyStatus::kError; }
}  // namespace mk