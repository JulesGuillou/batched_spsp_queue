add_executable(batched_spsc_queue_benchmarks benchmarks.cc)

set_target_properties(batched_spsc_queue_benchmarks PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

target_compile_options(batched_spsc_queue_benchmarks PRIVATE
  -Wall
  -Wextra
  -Wpedantic
)

target_link_libraries(batched_spsc_queue_benchmarks PRIVATE
  benchmark::benchmark
  batched_spsc_queue
)
