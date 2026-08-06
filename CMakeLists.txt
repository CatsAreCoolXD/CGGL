cmake_minimum_required(VERSION 3.28)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(CGGL
    VERSION 0.1.0
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Dependencies
add_subdirectory(deps)
add_subdirectory(deps/freetype)

# Library source files
add_library(${PROJECT_NAME} STATIC
    src/draw.cpp
    src/gui.cpp
    src/input.cpp
    src/scene.cpp
    src/shape.cpp
    src/style.cpp
    src/text.cpp
    src/types.cpp
    src/window.cpp
        include/CGGL/raytracing/raytracing.h
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_20
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        glfw
        glad
        glm
        freetype
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O2>
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC CGGL_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS ${PROJECT_NAME} glfw glad glm glm-header-only freetype
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
        DIRECTORY ${glad_BINARY_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY ${glfw_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY ${glm_SOURCE_DIR}/glm
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)