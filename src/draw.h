#include "types.h"

namespace cg {
    void InitializeDrawing();
    void NewFrame();

    void Draw();
    void DrawTriangles(std::vector<float> verticesToDraw, std::vector<unsigned int> indicesToDraw, int texture = -1);

    class Texture;
    void Draw(cg::Texture& tex, int flags = 0);

    void PushTriangle(float* triangle);

    double GetDeltatime();
    bool IsUsingTexture();
    cg::Vec2f GetTextureOrigin();
    cg::Vec2f GetTextureSize();
    cg::Color GetTextureTint();
    cg::Vec2f GetTextureFlip();

    // Set the FPS limit, which is 60 by default.
    void SetFPSLimit(unsigned int fpsLimit);
    void SetBackgroundColor(cg::Color newColor);

    // Set the rendering mode. Choose between WIREFRAME_MODE and FILL_MODE. FILL_MODE is default
    void SetRenderingMode(int mode);

    class Texture {
        public:
            Texture() {}
            Texture(const char* path, int wrapping = REPEAT);

            void LoadImage(const char* path, int wrapping = REPEAT);

            void SetOrigin(cg::Vec2f pos, bool centered = false);
            void SetSize(cg::Vec2f newSize);
            void Scale(float scale);
            void SetTint(cg::Color col) { tint = col; }

            cg::Vec2f GetOrigin() const { return origin; }
            cg::Vec2f GetSize() const { return size; }
            cg::Color GetTint() const { return tint; }

            cg::Vec2f GetOriginNDC() const { return originNDC; }
            cg::Vec2f GetSizeNDC() const { return sizeNDC; }

            void FlipHorizontally() { flip.x = 1.f; }
            void FlipVertically() { flip.y = 1.f; }

            int nrChannels;
            unsigned int textureId;
            cg::Vec2f flip;
        private:
            cg::Vec2f origin, originNDC, size, sizeNDC;
            cg::Color tint;
            int width, height;
    };

    void UseTexture(cg::Texture& tex);
    void StopUsingTexture();
}