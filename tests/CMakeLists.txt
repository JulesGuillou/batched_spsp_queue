add_executable(batched_spsc_queue_tests capacity_tests.cc multithread_tests.cc)

set_target_properties(batched_spsc_queue_tests PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

target_compile_options(batched_spsc_queue_tests PRIVATE
  -Wall
  -Wextra
  -Wpedantic
)

target_link_libraries(batched_spsc_queue_tests PRIVATE
  gtest_main
  batched_spsc_queue
)

include(GoogleTest)
gtest_discover_tests(batched_spsc_queue_tests)
