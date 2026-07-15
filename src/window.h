#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include "types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define RENDERING_MODE_WIREFRAME GL_LINE
#define RENDERING_MODE_FILL GL_FILL

namespace cg {
    // Initialize DiamondGUI.
    void Initialize(std::string windowName = "Diamond GUI Window", cg::Vec2i size = cg::Vec2i{1920, 1080}, bool resizable = false);

    GLFWwindow* GetWindow();
    double GetDeltatime();

    // Set the FPS limit, which is 60 by default.
    void SetFPSLimit(unsigned int fpsLimit);
    void SetBackgroundColor(cg::Color newColor);
    
    // Set the rendering mode. Choose between WIREFRAME_MODE and FILL_MODE. FILL_MODE is default
    void SetRenderingMode(int mode);

    void NewFrame();

    bool WindowIsOpen();

    void PushTriangle(float* triangle);

    void Draw();

    void Terminate();
};

#endif