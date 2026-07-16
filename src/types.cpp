#include "types.h"
#include "window.h"

#include <string>
#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace cg {
    float clamp(float v, float min, float max){
        if (v < min) return min;
        if (v > max) return max;
        return v;
    }

    Shader::Shader(const char* vertexPath, const char* fragmentPath){
        Shader::Create(vertexPath, fragmentPath);
        created = true;
    }

    void Shader::Create(const char* vertexPath, const char* fragmentPath){
        // Create shader program
        id = glCreateProgram();

        // Compile and link vertex and fragment shaders
        unsigned int vertexId, fragmentId;
        Shader::CompileShader(vertexPath, vertexId, GL_VERTEX_SHADER);
        Shader::CompileShader(fragmentPath, fragmentId, GL_FRAGMENT_SHADER);

        glAttachShader(id, vertexId);
        glAttachShader(id, fragmentId);
        glLinkProgram(id);

        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete shaders since we've already linked them and don't need them anymore
        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);
    }

    void Shader::CompileShader(const char* path, unsigned int& shaderId, int shaderType){
        std::ifstream file(path);
        std::string source, line;
        while (std::getline(file, line))
            source += line + "\n";

        const char* vertexShaderSource = source.c_str();
        file.close();

        shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, &vertexShaderSource, NULL);
        glCompileShader(shaderId);

        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "ERROR: FAILED TO COMPILE SHADER:\n" << infoLog << std::endl;
        }
    }

    void Shader::Use(){
        glUseProgram(id);
    }

    void Shader::SetBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }
    void Shader::SetInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }
    void Shader::SetFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
}