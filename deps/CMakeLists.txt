include(FetchContent)

# glfw
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

set(glfw_SOURCE_DIR "${glfw_SOURCE_DIR}" PARENT_SCOPE)

# glad
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v0.1.36
)

FetchContent_MakeAvailable(glad)

set(glad_SOURCE_DIR "${glad_SOURCE_DIR}" PARENT_SCOPE)
set(glad_BINARY_DIR "${glad_BINARY_DIR}" PARENT_SCOPE)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)

set(glm_SOURCE_DIR "${glm_SOURCE_DIR}" PARENT_SCOPE)