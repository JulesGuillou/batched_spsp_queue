add_library(batched_spsc_queue STATIC batched_spsc_queue.cc)

set_target_properties(batched_spsc_queue PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(batched_spsc_queue PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_include_directories(batched_spsc_queue PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
