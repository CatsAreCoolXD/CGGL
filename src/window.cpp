#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "window.h"
#include "types.h"
#include "style.h"
#include "draw.h"

namespace cg {
    namespace {
        GLFWwindow* window = nullptr;

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    }

    void Initialize(std::string windowName, cg::Vec2i size, bool resizable){
        if (!glfwInit()){
            throw std::runtime_error("Couldn't initialize GLFW");
        }

        glfwWindowHint(GLFW_RESIZABLE, resizable);

        window = glfwCreateWindow(size.x, size.y, windowName.c_str(), NULL, NULL);
        if (!window){
            glfwTerminate();
            throw std::runtime_error("Couldn't open window");
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            throw std::runtime_error("Couldn't load OpenGL function pointers");
        }

        glViewport(0, 0, size.x, size.y);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        cg::InitializeDrawing();
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
    }
}

