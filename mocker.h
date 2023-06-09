#pragma once

#include <SDL3/SDL.h>

#include <boost/di.hpp>
#include <memory>
#include <vector>

#include "base/run_loop_backend_executor.h"
#include "di_names.h"
#include "ui_application.h"

namespace mk {
class TaskLoop;
class FilesystemBrowserView;
class DispatchTask;
class ImageView;

class Mocker : public UiApplication {
 public:
  BOOST_DI_INJECT(
      Mocker,
      (named = di_names::UiRunLoop) std::shared_ptr<TaskLoop> ui_task_loop,
      (named = di_names::UiDispathTask) std::shared_ptr<DispatchTask>
          ui_task_dispatcher,
      (named = di_names::FilesystemRunLoop) std::shared_ptr<TaskLoop>
          filesystem_task_loop,
      (named = di_names::FilesystemDispatchTask) std::shared_ptr<DispatchTask>
          filesystem_task_dispatcher,
      std::shared_ptr<RunLoopBackendExecutor> ui_backend_executor,
      std::shared_ptr<FilesystemBrowserView> filesystem_browser);

  /** @see UiApplication. */
  UiApplication::Status Run() override;

 private:
  UiApplication::Status Initialize();
  RunLoopBackendExecutor::IterationStatus DrawUi();

  std::shared_ptr<TaskLoop> ui_task_loop_;
  std::shared_ptr<TaskLoop> filesystem_task_loop_;
  std::shared_ptr<DispatchTask> filesystem_task_dispatcher_;
  std::shared_ptr<DispatchTask> ui_task_dispatcher_;
  std::shared_ptr<RunLoopBackendExecutor> ui_backend_executor_;
  std::shared_ptr<FilesystemBrowserView> filesystem_browser_;

  SDL_GLContext gl_context_;
  SDL_Window* window_;
  bool show_demo_window_;

  std::vector<std::shared_ptr<ImageView>> selected_images_;
};
}  // namespace mk