#ifndef DRAW_H
#define DRAW_H

#include <unordered_map>

#include "types.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define NORMAL_RENDERING 0
#define TEXTURE_RENDERING 1
#define GLYPH_RENDERING 2

#define USAGE_DYNAMIC GL_DYNAMIC_DRAW
#define USAGE_STATIC GL_STATIC_DRAW
#define USAGE_STREAM GL_STREAM_DRAW

namespace cg {
    class Texture;
    class TriangleBuffer {
        public:
            TriangleBuffer() {}
            TriangleBuffer(bool isTexture, cg::Texture* tex = nullptr) : isTexture(isTexture), texture(tex) {}

            std::vector<float>& GetVertices() { return vertices; }
            std::unordered_map<size_t, std::vector<unsigned int>>& GetVerticesMap() { return verticesMap; }
            std::vector<unsigned int>& GetIndices() { return indices; }
            cg::Shader* GetShader() { return shader; }

            bool IsTexture() const { return isTexture; }
            cg::Texture* GetTexture() const { return texture; }
            void SetTexture(cg::Texture* tex) { texture = tex; isTexture = true; }
            void SetShader(cg::Shader* shader) { this->shader = shader; }
        private:
            std::vector<float> vertices;
            std::unordered_map<size_t, std::vector<unsigned int>> verticesMap;
            std::vector<unsigned int> indices;
            bool isTexture = false;
            cg::Texture* texture = nullptr;
            cg::Shader* shader;
    };

    size_t HashVertex(float* vertex, int size);

    void InitializeDrawing();
    void NewFrame();

    void Draw();
    void DrawTriangles(cg::TriangleBuffer& triangles);

    class Texture;
    void Draw(cg::Texture& tex, int flags = 0);
    void Draw(cg::Texture& tex, cg::Vec2f origin, cg::Vec2f size, int flags = 0);

    class Scene;
    void Draw(cg::Scene& scene, cg::Vec3f cameraPos);

    void DrawFullscreenShader(cg::Shader* shader, cg::Texture* tex = nullptr);

    void PushTriangle(float* triangle);
    cg::TriangleBuffer& NewTriangleBuffer();

    int GetVerticesAmount();
    int GetIndicesAmount();
    int GetTexturesAmount();

    GLuint GetUsage();

    double GetDeltatime();
    double GetAverageFPS();
    bool IsUsingTexture();
    cg::Vec2f GetTextureOrigin();
    cg::Vec2f GetTextureSize();
    cg::Color GetTextureTint();
    cg::Vec2f GetTextureFlip();
    cg::Vec2f GetTextureOffset();
    cg::Texture* GetCurrentTexture();

    cg::TriangleBuffer& GetActiveTriangleBuffer();

    // Set the FPS limit, which is 60 by default.
    void SetFPSLimit(unsigned int fpsLimit);
    void SetBackgroundColor(cg::Color newColor);

    cg::Color GetBackgroundColor();

    // Set the rendering mode. Choose between WIREFRAME_MODE and FILL_MODE. FILL_MODE is default.
    void SetRenderingMode(int mode);

    // Set the vertex usage mode. Default is USAGE_DYNAMIC.
    void SetUsageMode(int newUsage);

    class Texture {
        public:
            Texture() {}
            Texture(const char* path, int wrapping = REPEAT);

            void LoadImage(const char* path, int wrapping = REPEAT);
            void LoadGlyph(FT_Face& face);

            void SetOrigin(cg::Vec2f pos, bool centered = false);
            void SetSize(cg::Vec2f newSize);
            void Scale(float scale);
            void SetTint(cg::Color col) { tint = col; }

            cg::Vec2f GetOrigin() const { return origin; }
            cg::Vec2f GetSize() const { return size; }
            cg::Color GetTint() const { return tint; }
            bool IsGlyph() const { return isGlyph; }

            cg::Vec2f GetOriginNDC() const { return originNDC; }
            cg::Vec2f GetSizeNDC() const { return sizeNDC; }

            void FlipHorizontally() { flip.x = 1.f; }
            void FlipVertically() { flip.y = 1.f; }

            int nrChannels;
            unsigned int textureId;
            cg::Vec2f flip, offset;
        private:
            cg::Vec2f origin, originNDC, size, sizeNDC;
            cg::Color tint;
            int width, height;
            bool isGlyph = false;
    };

    void UseTexture(cg::Texture& tex);
    void StopUsingTexture();

    void UseShaderProgram();
    void PushNewTriangleBuffer(cg::Texture* tex = nullptr, cg::Shader* shader = nullptr);
}

#endif