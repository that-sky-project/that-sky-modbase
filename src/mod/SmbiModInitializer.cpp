#include <thread>
#include <vector>
#include <algorithm>
#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"

#undef max

class SmbiModInitThreadPool: public SmbiMutexObject {
  using PCInit = const SmbiModInitializer *;
  using TaskPayload = std::vector<PCInit>;
  using Task = std::pair<std::thread, TaskPayload>;

public:
  SmbiModInitThreadPool() = default;

  ~SmbiModInitThreadPool() { Terminate(); }

  void Initialize() {
    m_index = 0;
    u32 concurrency = std::thread::hardware_concurrency();
    m_concurrency = std::max(2u, concurrency) - 1;
    m_tasks.resize(m_concurrency);
    m_running = false;
  }

  void Terminate() {
    std::vector<std::thread> threads;
    {
      auto lock = LockRW();
      for (auto &task: m_tasks) {
        if (task.first.joinable())
          threads.push_back(std::move(task.first));
      }
      m_tasks.clear();
    }
    for (auto &t: threads) {
      if (t.joinable())
        t.join();
    }

    m_running = false;
  }

  void Submit(
    const SmbiModInitializer *task
  ) {
    auto lock = LockRW();

    if (m_running)
      return;

    m_tasks[m_index].second.push_back(task);
    m_index++;
    m_index %= m_concurrency;
  }

  void Start() {
    auto lock = LockRW();

    if (m_running)
      return;

    m_running = true;

    for (auto &task: m_tasks) {
      task.first = std::thread(m_ThreadFunc, &task.second);
    }
  }

  void StartSync() {
    Start();
    Terminate();
  }

private:
  static void m_ThreadFunc(
    const TaskPayload *funcs
  ) {
    for (auto &fn: *funcs) {
      (*fn)(hModuleDll);
    }
  }

  u32 m_concurrency;
  u32 m_index;
  std::vector<Task> m_tasks;
  bool m_running;
};

HTStatus SmbiModInitializer::SetupAll(
  HMODULE hModuleDll
) {
  using PCInit = const SmbiModInitializer *;
  HTStatus s = HT_SUCCESS;

  for (PCInit p = SmbiModInitializer::list(); p; p = p->GetPrev()) {
    if (!(*p)(hModuleDll))
      s = HT_FAIL;
  }

  return s;
}
