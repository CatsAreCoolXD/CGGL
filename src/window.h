#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include "types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define RENDERING_MODE_WIREFRAME GL_LINE
#define RENDERING_MODE_FILL GL_FILL

namespace cg {
    // Initialize CGGL.
    void Initialize(std::string windowName = "CGGL Window", cg::Vec2i size = cg::Vec2i{1920, 1080}, bool resizable = false);

    GLFWwindow* GetWindow();
    
    bool WindowIsOpen();

    void Terminate();
};

#endif