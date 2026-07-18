#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "window.h"
#include "types.h"
#include "style.h"
#include "draw.h"
#include "input.h"
#include "text.h"

namespace cg {
    namespace {
        GLFWwindow* window = nullptr;

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        void mouse_scroll_callback(GLFWwindow* window, double x, double y){
            cg::Input::SetMouseScroll(cg::Vec2d(x, y));
        }
    }

    void Initialize(std::string windowName, cg::Vec2i size, cg::WindowSettings settings){
        if (!glfwInit()){
            throw std::runtime_error("Couldn't initialize GLFW");
        }

        glfwWindowHint(GLFW_RESIZABLE, settings.resizable);

        if (settings.antiAliasing){
            glfwWindowHint(GLFW_SAMPLES, settings.antiAliasingLevel);
        }

        if (settings.windowMode == WINDOW_MODE_WINDOWED) window = glfwCreateWindow(size.x, size.y, windowName.c_str(), NULL, NULL);
        else if (settings.windowMode == WINDOW_MODE_FULLSCREEN) window = glfwCreateWindow(size.x, size.y, windowName.c_str(), glfwGetPrimaryMonitor(), NULL); 
        else if (settings.windowMode == WINDOW_MODE_FULLSCREEN_BORDERLESS){
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
 
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), glfwGetPrimaryMonitor(), NULL); 
        }
        if (!window){
            glfwTerminate();
            throw std::runtime_error("Couldn't open window");
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            throw std::runtime_error("Couldn't load OpenGL function pointers");
        }

        if (settings.antiAliasing)
            glEnable(GL_MULTISAMPLE);

        glViewport(0, 0, size.x, size.y);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetScrollCallback(window, mouse_scroll_callback);

        cg::InitializeDrawing();
        cg::InitiliazeFreeType();
    }

    GLFWwindow* GetWindow() {
        return window;
    }

    bool WindowIsOpen(){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Quit if the escape key was pressed
            glfwSetWindowShouldClose(window, true);
        return !glfwWindowShouldClose(window);
    }

    void Terminate(){
        glfwTerminate();
        cg::ShutdownFreetype();
    }
}

