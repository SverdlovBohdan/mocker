#include <boost/di.hpp>
#include <memory>

#include "base/dispatch_task.h"
#include "base/priority_task_queue.h"
#include "base/run_loop.h"
#include "base/run_loop_backend_executor.h"
#include "base/run_loop_ui.h"
#include "base/steady_time_provider.h"
#include "base/task_pump_std.h"
#include "di_names.h"
#include "filesystem_browser.h"
#include "filesystem_browser_view.h"
#include "mocker.h"
#include "ui_application.h"

int main(int, char**) {
  using namespace mk;
  using namespace boost;

  const auto injector = di::make_injector(
      di::bind<TaskPump>.to<TaskPumpStd>(),
      di::bind<TaskQueue>.to<PriorityTaskQueue>(),
      di::bind<TimeProvider>.to<SteadyTimeProvider>(),
      di::bind<TaskLoop>().named(di_names::UiRunLoop).to<RunLoopUi>(),
      di::bind<DispatchTask>().named(di_names::UiDispathTask).to<RunLoopUi>(),
      di::bind<TaskLoop>().named(di_names::FilesystemRunLoop).to<RunLoop>(),
      di::bind<DispatchTask>()
          .named(di_names::FilesystemDispatchTask)
          .to<RunLoop>(),
      di::bind<RunLoopBackendExecutor>.to<RunLoopUi>(),
      di::bind<FilesystemReader, FilesystemBrowserView>.to<FilesystemBrowser>(),
      di::bind<UiApplication>.to<Mocker>());

  auto mocker = injector.create<std::shared_ptr<UiApplication>>();
  mocker->Run();
  return 0;
}