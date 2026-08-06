#include <iostream>
#include <chrono>
#include <string>

#include "CGGL/window.h"
#include "CGGL/types.h"
#include "CGGL/draw.h"
#include "CGGL/input.h"
#include "CGGL/text.h"

namespace cg {
    namespace {
        GLFWwindow* window = nullptr;

        void mouse_scroll_callback(GLFWwindow* window, double x, double y){
            cg::SetMouseScroll(cg::Vec2d(x, y));
        }
        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    }

    cg::Vec2i GetWindowSize(){
        int w,h;
        glfwGetWindowSize(window, &w, &h);
        return cg::Vec2i(w,h);
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

        int left, top, right, bottom;
        glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);

        glViewport(0, 0, size.x, size.y - top + left);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetWindowSize(window, size.x, size.y);

        glfwSetScrollCallback(window, mouse_scroll_callback);

        cg::InitializeDrawing();
        cg::InitiliazeFreeType();
    }

    void ToggleVSync(bool on){
        glfwSwapInterval(on);
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

