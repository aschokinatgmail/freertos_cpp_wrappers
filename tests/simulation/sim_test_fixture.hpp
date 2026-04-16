#pragma once

#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <signal.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

namespace freertos_sim {

using TestFunction = void (*)();

inline int run_in_subprocess(TestFunction fn, int timeout_ms = 10000) {
  pid_t pid = fork();
  if (pid == 0) {
    fn();
    vTaskEndScheduler();
    _exit(EXIT_SUCCESS);
  } else if (pid > 0) {
    int status;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000L;
    sigtimedwait(&mask, nullptr, &ts);

    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
      kill(pid, SIGKILL);
      waitpid(pid, &status, 0);
      return -1;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
  }
  return -1;
}

} // namespace freertos_sim

#define FREERTOS_SIM_TEST(test_name, ...)                                      \
  TEST(SimulationSmoke, test_name) {                                           \
    int result = freertos_sim::run_in_subprocess(__VA_ARGS__);                 \
    ASSERT_EQ(result, 0) << "Simulation test " #test_name                      \
                         << " failed or timed out";                            \
  }