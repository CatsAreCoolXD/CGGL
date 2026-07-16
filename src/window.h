#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include "types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define RENDERING_MODE_WIREFRAME GL_LINE
#define RENDERING_MODE_FILL GL_FILL

#define WINDOW_MODE_WINDOWED 0
#define WINDOW_MODE_FULLSCREEN 1
#define WINDOW_MODE_FULLSCREEN_BORDERLESS 2

#define WINDOW_FLAG_RESIZABLE 1u
#define WINDOW_FLAG_

namespace cg {
    struct WindowSettings {
        bool resizable = false, antiAliasing = false;
        int antiAliasingLevel = 8;
        int windowMode = WINDOW_MODE_WINDOWED;
    };

    // Initialize CGGL.
    void Initialize(std::string windowName = "CGGL Window", cg::Vec2i size = cg::Vec2i{1920, 1080}, cg::WindowSettings settings = cg::WindowSettings());

    GLFWwindow* GetWindow();
    
    bool WindowIsOpen();

    void Terminate();
};

#endif