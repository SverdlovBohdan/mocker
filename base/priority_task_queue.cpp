#include "priority_task_queue.h"

#include <cassert>

namespace mk {
TaskHandle PriorityTaskQueue::AddTask(std::shared_ptr<PendingTask> task) {
  TaskHandle handle{task};
  queue_.push(std::move(task));

  return handle;
}

std::shared_ptr<PendingTask> PriorityTaskQueue::PopTask() {
  assert(!IsEmpty() && "PopTask(). PriorityTaskQueue is empty.");

  auto pending_task = queue_.top();
  queue_.pop();

  return pending_task;
}

bool PriorityTaskQueue::IsEmpty() const { return queue_.empty(); }

TimestampMs PriorityTaskQueue::GetNextTaskCallTime() const {
  assert(!IsEmpty() && "GetNextTaskCallTime(). PriorityTaskQueue is empty.");

  return queue_.top()->next_call;
}
}  // namespace mk