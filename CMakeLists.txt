cmake_minimum_required(VERSION 3.28)
project(CGGL)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# glad, glfw
add_subdirectory(deps)

# freetype
add_subdirectory(deps/freetype)

add_executable(main)

target_sources(main
    PRIVATE
        src/example.cpp src/window.cpp src/style.cpp src/shape.cpp src/types.cpp src/draw.cpp src/input.cpp src/text.cpp
)

target_link_libraries(main PRIVATE glfw glad freetype)