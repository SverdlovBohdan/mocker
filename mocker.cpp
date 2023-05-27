#include "mocker.h"

#include <SDL3/SDL.h>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include "base/task_loop.h"

namespace mk {
Mocker::Mocker(std::shared_ptr<TaskLoop> ui_task_loop,
               std::shared_ptr<RunLoopBackendExecutor> backend_executor)
    : ui_task_loop_{std::move(ui_task_loop)},
      backend_executor_{std::move(backend_executor)},
      gl_context_{nullptr},
      window_{nullptr},
      show_demo_window_{false} {}

UiApplication::Status Mocker::Run() {
  if (const auto status = Initialize(); status != UiApplication::Status::Ok) {
    return status;
  }

  backend_executor_->SetBackendTask([this]() { return DrawUi(); });
  ui_task_loop_->Run();

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context_);
  SDL_DestroyWindow(window_);
  SDL_Quit();

  return UiApplication::Status::Ok;
}

UiApplication::Status Mocker::Initialize() {
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0) {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return UiApplication::Status::Error;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Enable native IME.
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                        SDL_WINDOW_HIDDEN);
  window_ = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", 1280, 720,
                             window_flags);
  if (window_ == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return UiApplication::Status::Error;
  }

  SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  gl_context_ = SDL_GL_CreateContext(window_);
  SDL_GL_MakeCurrent(window_, gl_context_);
  SDL_GL_SetSwapInterval(1);  // Enable vsync
  SDL_ShowWindow(window_);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForOpenGL(window_, gl_context_);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return UiApplication::Status::Ok;
}

RunLoopBackendExecutor::IterationStatus Mocker::DrawUi() {
  bool done{false};
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGuiIO& io = ImGui::GetIO();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
      done = true;
    }
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event.window.windowID == SDL_GetWindowID(window_)) {
      done = true;
    }
  }

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  if (show_demo_window_) ImGui::ShowDemoWindow(&show_demo_window_);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to create a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
                                    // and append into it.

    ImGui::Text("This is some useful text.");  // Display some text (you can use
                                               // a format strings too)
    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float*)&clear_color);  // Edit 3 floats representing a color

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most
                                  // widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  //   if (show_another_window) {
  //     ImGui::Begin(
  //         "Another Window",
  //         &show_another_window);  // Pass a pointer to our bool variable (the
  //                                 // window will have a closing button that
  //                                 will
  //                                 // clear the bool when clicked)
  //     ImGui::Text("Hello from another window!");
  //     if (ImGui::Button("Close Me")) show_another_window = false;
  //     ImGui::End();
  //   }

  // Rendering
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(window_);

  return done ? RunLoopBackendExecutor::IterationStatus::Done
              : RunLoopBackendExecutor::IterationStatus::Ok;
}
}  // namespace mk