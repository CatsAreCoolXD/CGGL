#ifndef DG_MATH_H
#define DG_MATH_H

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
    class Vec3; // forward declare Vec3 so Vec2 can reference it
    template<typename T>
    class Vec2 {
        public:
            Vec2() : x(0), y(0) {}
            Vec2(T x, T y) : x(x), y(y) {}

            float Length() {
                return std::sqrt(x*x + y*y);
            }
            Vec2 Normalized() {
                return (*this) / Length();
            }

            template<typename U>
            Vec2(const Vec3<U>& other) : x((T)other.x), y((T)other.y) {}

            template<typename U>
            Vec2(const Vec2<U>& other) : x((T)other.x), y((T)other.y) {}
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
    Vec2<T> operator/(Vec2<T> l, float r) {
        return Vec2<T>(l.x / r, l.y / r);
    }

    template<typename T>
    Vec2<T> operator*(Vec2<T> l, float r) {
        return Vec2<T>(l.x * r, l.y * r);
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

            template<typename U>
            Vec3(const Vec3<U>& other) : x((T)other.x), y((T)other.y), z((T)other.z) {}
            
            template<typename U>
            Vec3(const Vec2<U>& other) : x((T)other.x), y((T)other.y), z((T)0) {}

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

            T x, y, z;
            T &r = x, &g = y, &b = z;
    };

    template<typename T>
    Vec3<T> operator+(Vec3<T> l, Vec3<T> r) {
        return Vec3<T>(l.x + r.x, l.y + r.y, l.z + r.z);
    }

    template<typename T>
    Vec3<T> operator-(Vec3<T> l, Vec3<T> r) {
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

    using Vec3f = Vec3<float>;
    using Vec3i = Vec3<int>;
    using Vec3d = Vec3<double>;
    using Vec3u = Vec3<unsigned int>;

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
    };

    class Vertex {
        public:
            Vertex() : pos(cg::Vec3f()), color(cg::Color()) {}
            Vertex(cg::Vec3f pos, cg::Color color) : pos(pos), color(color) {}
            Vertex(cg::Vec2f pos, cg::Color color) : pos(pos), color(color) {}
            cg::Vec3f pos;
            cg::Color color;

            Vertex& operator=(const Vertex& other) {
                pos = other.pos;
                color = other.color;
                return *this;
            }
    };

    class Shader {
        public:
            Shader() {}
            Shader(const char* vertexPath, const char* fragmentPath);

            void Create(const char* vertexPath, const char* fragmentPath);
            void Use();
            void SetBool(const std::string& name, bool value) const;
            void SetInt(const std::string& name, int value) const;
            void SetFloat(const std::string& name, float value) const;

            unsigned int id;
        private:
            void CompileShader(const char* path, unsigned int& shaderId, int shaderType);
            bool created = false;
    };

    float clamp(float v, float min, float max);
}

#define colorToFloat(color) (float)color.r/255.f, (float)color.g/255.f, (float)color.b/255.f, (float)color.a/255.f
#define clamp11(v) clamp(v, -1.f, 1.f)
#define clamp01(v) clamp(v, 0.f, 1.f)

#define PI 3.14159f

#endif