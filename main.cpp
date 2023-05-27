#include <memory>
#include <boost/di.hpp>

#include "base/dispatch_task.h"
#include "base/priority_task_queue.h"
#include "base/run_loop_backend_executor.h"
#include "base/run_loop_ui.h"
#include "base/steady_time_provider.h"
#include "mocker.h"
#include "ui_application.h"

int main(int, char**) {
  using namespace mk;
  using namespace boost;

  const auto injector = di::make_injector(
      di::bind<TaskQueue>.to<PriorityTaskQueue>(),
      di::bind<TimeProvider>.to<SteadyTimeProvider>(),
      di::bind<TaskLoop, DispatchTask, RunLoopBackendExecutor>.to<RunLoopUi>(),
      di::bind<UiApplication>.to<Mocker>());

  auto mocker = injector.create<std::shared_ptr<UiApplication>>();
  mocker->Run();
  return 0;
}