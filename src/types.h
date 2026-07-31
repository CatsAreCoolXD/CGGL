#ifndef DG_MATH_H
#define DG_MATH_H

#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <vector>

#include <glad/glad.h>

#define REPEAT GL_REPEAT
#define MIRRORED_REPEAT GL_MIRRORED_REPEAT
#define CLAMP_TO_EDGE GL_CLAMP_TO_EDGE
#define CLAMP_TO_BORDER GL_CLAMP_TO_BORDER

namespace cg {
    template<typename T>
    class Vec3; // Forward declare Vec3 so Vec2 can use it

    template<typename T>
    class Vec2 {
        public:
            Vec2() : x(0), y(0) {}
            Vec2(T x, T y) : x(x), y(y) {}

            float LengthSquared() {
                return x*x + y*y;
            }
            float Length(){
                return std::sqrt(Vec2::LengthSquared());
            }
            Vec2<T> Normalized() {
                return (*this) / Vec2::Length();
            }

            float Dot(Vec2 other){
                return x * other.x + y * other.y;
            }

            template<typename U>
            Vec2(const Vec3<U>& other) : x((T)other.x), y((T)other.y) {}

            template<typename U>
            Vec2(const Vec2<U>& other) : x((T)other.x), y((T)other.y) {}

            Vec2<T>& operator=(const Vec2<T>& other) {
                x = other.x;
                y = other.y;
                return *this;
            }

            template<typename U>
            Vec2<T>& operator=(const Vec2<U>& other) {
                x = (T)other.x;
                y = (T)other.y;
                return *this;
            }

            template<typename U>
            bool operator==(const Vec2<U>& other) const {
                return x == other.x && y == other.y;
            }

            T x, y;
    };

    template<typename T>
    Vec2<T> operator+(Vec2<T> l, Vec2<T> r) {
        return Vec2<T>(l.x + r.x, l.y + r.y);
    }

    template<typename T>
    Vec2<T> operator-(Vec2<T> l, Vec2<T> r) {
        return Vec2<T>(l.x - r.x, l.y - r.y);
    }

    template<typename T>
    Vec2<T> operator-(Vec2<T> l, float r) {
        return Vec2<T>(l.x - r, l.y - r);
    }

    template<typename T>
    Vec2<T> operator/(Vec2<T> l, float r) {
        return Vec2<T>(l.x / r, l.y / r);
    }

    template<typename T>
    Vec2<T> operator*(Vec2<T> l, float r) {
        return Vec2<T>(l.x * r, l.y * r);
    }

    template<typename T>
    Vec2<T> operator/(Vec2<T> l, Vec2<T>r) {
        return Vec2<T>(l.x / r.x, l.y / r.y);
    }

    template<typename T>
    Vec2<T> operator*(Vec2<T> l, Vec2<T> r) {
        return Vec2<T>(l.x * r.x, l.y * r.y);
    }

    using Vec2f = Vec2<float>;
    using Vec2i = Vec2<int>;
    using Vec2d = Vec2<double>;
    using Vec2u = Vec2<unsigned int>;

    template<typename T>
    class Vec3 {
        public:
            Vec3() : x(0), y(0), z(0) {}
            Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

            T x, y, z;
            T &r = x, &g = y, &b = z;

            template<typename U>
            Vec3(const Vec3<U>& other) : x((T)other.x), y((T)other.y), z((T)other.z) {}
            
            template<typename U>
            Vec3(const Vec2<U>& other) : x((T)other.x), y((T)other.y), z((T)0) {}

            float LengthSquared() {
                return x*x + y*y + z*z;
            }
            float Length(){
                return std::sqrt(Vec3::LengthSquared());
            }
            Vec3<T> Normalized() {
                return (*this) / Vec3::Length();
            }

            template<typename U>
            float Dot(Vec3<U> other){
                return x * (T)other.x + y * (T)other.y + z * (T)other.z;
            }

            template<typename U>
            Vec3<T> Cross(Vec3<U> other){
                return Vec3<T>(y * (T)other.z - b * (T)other.y, z * (T)other.x - x * (T)other.z, x * (T)other.y - y * (T)other.x);
            }

            Vec3<T>& operator=(const Vec3<T>& other) {
                x = other.x;
                y = other.y;
                z = other.z;
                return *this;
            }

            template<typename U>
            Vec3<T>& operator=(const Vec3<U>& other) {
                x = (T)other.x;
                y = (T)other.y;
                z = (T)other.z;
                return *this;
            }

            template<typename U>
            bool operator==(const Vec3<U>& other) const {
                return x == other.x && y == other.y && z == other.z;
            }
    };

    template<typename T>
    Vec3<T> operator+(Vec3<T> l, Vec3<T> r) {
        return Vec3<T>(l.x + r.x, l.y + r.y, l.z + r.z);
    }

    template<typename T>
    Vec3<T> operator-(Vec3<T>&l, Vec3<T> r) {
        return Vec3<T>(l.x - r.x, l.y - r.y, l.z - r.z);
    }

    template<typename T>
    Vec3<T> operator+(Vec3<T> l, Vec2<T> r) {
        return Vec3<T>(l.x + r.x, l.y + r.y, l.z);
    }

    template<typename T>
    Vec3<T> operator-(Vec3<T> l, Vec2<T> r) {
        return Vec3<T>(l.x - r.x, l.y - r.y, l.z);
    }

    template<typename T>
    Vec3<T> operator/(Vec3<T> l, float r) {
        return Vec3<T>(l.x / r, l.y / r, l.z / r);
    }

    template<typename T>
    Vec3<T> operator*(Vec3<T> l, float r) {
        return Vec3<T>(l.x * r, l.y * r, l.z * r);
    }

    template<typename T>
    Vec3<T> operator/(Vec3<T> l, Vec3<T> r) {
        return Vec3<T>(l.x / r.x, l.y / r.y, l.z / r.z);
    }

    template<typename T>
    Vec3<T> operator*(Vec3<T> l, Vec3<T> r) {
        return Vec3<T>(l.x * r.x, l.y * r.y, l.z / r.z);
    }

    using Vec3f = Vec3<float>;
    using Vec3i = Vec3<int>;
    using Vec3d = Vec3<double>;
    using Vec3u = Vec3<unsigned int>;

    template<typename T>
    void PrintVector(Vec3<T> v){
        std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    }

    class Color {
        public:
            Color() : r(0.f), g(0.f), b(0.f), a(255.f) {}
            Color(float r, float g, float b, float a = 255.f) : r(r), g(g), b(b), a(a) {}
            float r, g, b, a;

            Color& operator=(const Color& other){
                r = other.r;
                g = other.g;
                b = other.b;
                a = other.a;
                return *this;
            }
            
            bool operator==(const Color& other) const {
                return r == other.r && g == other.g && b == other.b && a == other.a;
            }
    };

    cg::Color operator*(cg::Color l, cg::Color r);
    cg::Color operator*(cg::Color l, float r);

    cg::Color operator/(cg::Color l, cg::Color r);
    cg::Color operator/(cg::Color l, float r);

    cg::Color operator+(cg::Color l, cg::Color r);
    cg::Color operator+(cg::Color l, float r);

    cg::Color operator-(cg::Color l, cg::Color r);
    cg::Color operator-(cg::Color l, float r);

    class Vertex {
        public:
            Vertex() : pos(cg::Vec3f()), color(cg::Color()) {}
            Vertex(cg::Vec3f pos, cg::Color color, cg::Vec2f texCoords = cg::Vec2f(0,0)) : pos(pos), color(color), texCoords(texCoords) {}
            Vertex(cg::Vec2f pos, cg::Color color, cg::Vec2f texCoords = cg::Vec2f(0,0)) : pos(pos), color(color), texCoords(texCoords) {}
            cg::Vec3f pos;
            cg::Color color;
            cg::Vec2f texCoords;

            // Make sure to delete data after use
            float* GetData(int size) const {
                if (size == 6){
                    float* vertex = new float[7];
                    vertex[0] = pos.x;
                    vertex[1] = pos.y;
                    vertex[2] = pos.z;
                    
                    vertex[3] = color.r;
                    vertex[4] = color.g;
                    vertex[5] = color.b;
                    return vertex;
                }
                float* vertex = new float[9];
                vertex[0] = pos.x;
                vertex[1] = pos.y;
                vertex[2] = pos.z;
                
                vertex[3] = color.r;
                vertex[4] = color.g;
                vertex[5] = color.b;

                vertex[6] = texCoords.x;
                vertex[7] = texCoords.y;

                return vertex;
            }

            Vertex& operator=(const Vertex& other) {
                pos = other.pos;
                color = other.color;
                return *this;
            }

            bool operator==(const Vertex& other) const {
                return pos == other.pos && color == other.color && texCoords == other.texCoords;
            }
    };

    class Texture;

    class Shader {
        public:
            Shader() {}
            Shader(const char* vertexPath, const char* fragmentPath);

            void CreateFromStrings(std::string vertexString, std::string fragmentString);
            void Create(const char* vertexPath, const char* fragmentPath);
            void Use();
            void SetBool(const std::string& name, bool value) const;
            void SetInt(const std::string& name, int value) const;
            void SetFloat(const std::string& name, float value) const;
            void SetFloats(const std::string& name, cg::Vec2f value) const;
            void SetFloats(const std::string& name, cg::Vec3f value) const;
            void SetFloats(const std::string& name, cg::Color value) const;
            void SetTexture(cg::Texture* tex) const;

            template <typename T>
            void SetArray(size_t size, T* list, unsigned int binding){
                GLuint ssbo;
                glGenBuffers(1, &ssbo);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
                glBufferData(GL_SHADER_STORAGE_BUFFER, size, list, GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            }

            unsigned int id;
        private:
            void CompileShader(const char* path, unsigned int& shaderId, int shaderType);
            void CompileShaderFromString(std::string code, unsigned int& shaderId, int shaderType);
            bool created = false;
    };

    float clamp(float v, float min, float max);
}

#define colorToFloat(color) (float)color.r/255.f, (float)color.g/255.f, (float)color.b/255.f, (float)color.a/255.f
#define clamp11(v) clamp(v, -1.f, 1.f)
#define clamp01(v) clamp(v, 0.f, 1.f)

#define PI 3.14159f

#endif