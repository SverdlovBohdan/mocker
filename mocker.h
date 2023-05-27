#pragma once

#include <memory>
#include <SDL3/SDL.h>

#include "ui_application.h"
#include "base/run_loop_backend_executor.h"

namespace mk {
class TaskLoop;
class RunLoopBackendExecutor;

class Mocker : public UiApplication {
 public:
  Mocker(std::shared_ptr<TaskLoop> ui_task_loop,
         std::shared_ptr<RunLoopBackendExecutor> backend_executor);

  /** @see UiApplication. */
  UiApplication::Status Run() override;

 private:
  UiApplication::Status Initialize();
  RunLoopBackendExecutor::IterationStatus DrawUi();

  std::shared_ptr<TaskLoop> ui_task_loop_;
  std::shared_ptr<RunLoopBackendExecutor> backend_executor_;

  SDL_GLContext gl_context_;
  SDL_Window* window_;
  bool show_demo_window_;
};
}  // namespace mk