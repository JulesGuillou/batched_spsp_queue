cmake_minimum_required(VERSION 3.27)
project(batched_spsc_queue)

# Default build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

option(BATCHED_SPSC_QUEUE_ENABLE_TESTING "Build tests" ON)

if (BATCHED_SPSC_QUEUE_ENABLE_TESTING)
    enable_testing()
endif ()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(docs)

