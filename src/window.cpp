#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "window.h"
#include "types.h"
#include "style.h"

namespace cg {
    namespace {
        GLFWwindow* window = nullptr;
        unsigned int framerateLimit = 60;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        cg::Color backgroundColor(255, 255, 255);

        double lastFrameTimePoint = glfwGetTime();
        double deltaTime = 0;

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        cg::Shader shaderProgram;
        GLuint VAO, VBO, EBO;
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

        // Create the shader program
        shaderProgram.Create("src/vertex.glsl", "src/fragment.glsl");
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Push default style to style stack
        cg::PushStyle(cg::GetStyle());
    }

    GLFWwindow* GetWindow() {
        return window;
    }

    double GetDeltatime(){
        return deltaTime;
    }

    void SetFPSLimit(unsigned int fpsLimit){
        framerateLimit = fpsLimit;
    }

    void SetBackgroundColor(cg::Color newColor) {
        backgroundColor = newColor;
    }

    void SetRenderingMode(int mode){
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }

    void NewFrame(){
        // Clear the vertexes and indices and allocate the same amount of memory, 
        // since the amount of memory used probably wont change much in between frames.
        int previousSize = vertices.size();
        vertices.clear();
        vertices.reserve(previousSize);
        
        previousSize = indices.size();
        indices.clear();
        indices.reserve(previousSize);

        glClearColor(colorToFloat(backgroundColor));
        glClear(GL_COLOR_BUFFER_BIT);

        // Check for events and update the window state
        glfwPollEvents();

        // Max frames per second
        while (true){
            double now = glfwGetTime();
            if (now - lastFrameTimePoint > 1.f / (float)framerateLimit)
                break;
        }

        double now = glfwGetTime();
        deltaTime = now - lastFrameTimePoint;
        lastFrameTimePoint = now;
    }

    bool WindowIsOpen(){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Quit if the escape key was pressed
            glfwSetWindowShouldClose(window, true);
        return !glfwWindowShouldClose(window);
    }

    void PushTriangle(float* triangle){
        for (int i = 0; i < 3; i++){
            float* vertex = &triangle[i * 6];

            int indice = vertices.size() / 6;

            // Look if this vertex has already been added to the list of vertices
            int pos = 0;
            bool foundVertex = false;
            for (int j = 0; j < vertices.size() / 6; j++){
                bool equal = true;
                for (int k = 0; k < 6; k++){
                    if (vertex[k] != vertices[j * 6 + k]){
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    foundVertex = true;
                    indice = j;
                    break;
                }
            }
            if (!foundVertex) { // Vertice is not a duplicate, so add it to the list.
                vertices.insert(vertices.end(), vertex, vertex + 6);
            }
            indices.push_back(indice);
        }
    }

    void Draw(){
        //std::cout << "Drawing " << vertices.size() << " vertices with " << indices.size() << " indices\n";
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Tell OpenGL how it should interpret vertex data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        // Use the shader program
        shaderProgram.Use();

        // Draw the triangles
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    void Terminate(){
        glfwTerminate();
    }
}

