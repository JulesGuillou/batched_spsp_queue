# Image manipulation
add_executable(images_manipulation images_manipulation.cc)

set_target_properties(images_manipulation PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(images_manipulation PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(images_manipulation PRIVATE
        batched_spsc_queue
)

# Minimal example
add_executable(minimal_example minimal_example.cc)

set_target_properties(minimal_example PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_compile_options(minimal_example PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_link_libraries(minimal_example PRIVATE
        batched_spsc_queue
)
