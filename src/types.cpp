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
    
    cg::Color operator*(cg::Color l, cg::Color r) {
        return cg::Color(l.r * r.r, l.g * r.g, l.b * r.b, l.a * r.a);
    }

    cg::Color operator*(cg::Color l, float r) {
        return cg::Color(l.r * r, l.g * r, l.b * r, l.a);
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
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "Shader error: Linking failed: \n" << infoLog << std::endl;
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

        const char* shaderSource = source.c_str();
        file.close();

        shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, &shaderSource, NULL);
        glCompileShader(shaderId);

        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "Failed to compile shader:\n" << infoLog << std::endl;
        }
    }

    void Shader::CreateFromStrings(std::string vertexString, std::string fragmentString){
        // Create shader program
        id = glCreateProgram();

        // Compile and link vertex and fragment shaders
        unsigned int vertexId, fragmentId;
        Shader::CompileShaderFromString(vertexString, vertexId, GL_VERTEX_SHADER);
        Shader::CompileShaderFromString(fragmentString, fragmentId, GL_FRAGMENT_SHADER);

        glAttachShader(id, vertexId);
        glAttachShader(id, fragmentId);
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if(!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            std::cout << "Shader error: Linking failed: \n" << infoLog << std::endl;
        }

        // Delete shaders since we've already linked them and don't need them anymore
        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);
    }

    void Shader::CompileShaderFromString(std::string code, unsigned int& shaderId, int shaderType){
        const char* shaderSource = code.c_str();

        shaderId = glCreateShader(shaderType);
        glShaderSource(shaderId, 1, &shaderSource, NULL);
        glCompileShader(shaderId);

        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "Failed to compile shader:\n" << infoLog << std::endl;
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
    void Shader::SetFloats(const std::string& name, cg::Vec2f value) const {
        glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
    }
    void Shader::SetFloats(const std::string& name, cg::Vec3f value) const {
        glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
    }
    void Shader::SetFloats(const std::string& name, cg::Color value) const {
        glUniform4f(glGetUniformLocation(id, name.c_str()), value.r, value.g, value.b, value.a);
    }
}