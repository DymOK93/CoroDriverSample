#pragma once
#include <bugcheck.h>
#include <exception.h>
#include <thread.h>
#include <coroutine.hpp>

auto switch_to_new_thread(ktl::system_thread& out) {
  using ktl::system_thread;

  struct awaitable {
    bool await_ready() { return false; }
    void await_suspend(ktl::coroutine_handle<> other) {
      ktl::throw_exception_if<ktl::runtime_error>(out_th->joinable(),
                                                  L"output thread is busy",
                                                  ktl::constexpr_message_tag{});
      *out_th = system_thread([other] { other.resume(); });
    }
    void await_resume() {}

    system_thread* out_th;
  };

  return awaitable{ktl::addressof(out)};
}

struct task {
  struct promise_type {
    task get_return_object() { return {}; }
    ktl::suspend_never initial_suspend() { return {}; }
    ktl::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};

task resuming_on_new_thread(ktl::system_thread& out);

TIME_FIELDS GetCurrentTime() noexcept;