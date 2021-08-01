#include "driver.h"

#include <string.hpp>

#include <fmt/xchar.h>

using namespace ktl;

EXTERN_C NTSTATUS DriverEntry([[maybe_unused]] PDRIVER_OBJECT DriverObject,
                              [[maybe_unused]] PUNICODE_STRING RegistryPath) {
  try {
    const auto time{GetCurrentTime()};
    auto str{format(L"[{:02}-{:02}-{:04} {:02}:{:02}:{:02}.{:03}][{}]",
                    time.Day, time.Month, time.Year, time.Hour, time.Minute,
                    time.Second, time.Milliseconds, L"CoroDriverSample")};
    DbgPrint("%wZ\n", str.raw_str());
    system_thread thread;
    resuming_on_new_thread(thread);
    thread.join();
  } catch (const exception& exc) {
    DbgPrint("Unhandled exception caught: %wS with code %x\n", exc.what(),
             exc.code());
    return exc.code();
  }
  return STATUS_INSUFFICIENT_RESOURCES;
}

task resuming_on_new_thread(system_thread& out) {
  DbgPrint("Coroutine started on thread: %u\n",
           HandleToUlong(PsGetCurrentThreadId()));
  co_await switch_to_new_thread(out);
  DbgPrint("Coroutine resumed on thread: %u\n",
           HandleToUlong(PsGetCurrentThreadId()));
}

TIME_FIELDS GetCurrentTime() noexcept {
  const auto current_time{
      chrono::system_clock::now().time_since_epoch().count()};
  LARGE_INTEGER native_time;
  native_time.QuadPart = static_cast<long long>(current_time);
  TIME_FIELDS time_fields;
  RtlTimeToTimeFields(addressof(native_time), addressof(time_fields));
  return time_fields;
}