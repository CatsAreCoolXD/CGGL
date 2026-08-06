#ifndef TEXT_H
#define TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stack>
#include <map>

#include "draw.h"

namespace cg
{
    struct FontCharacter {
        cg::Texture tex;
        unsigned int advance;
        cg::Vec2i size, bearing;
    };

    void InitiliazeFreeType();
    void ShutdownFreetype();

    class Font {
        public:
            Font() {}
            Font(const char* path, int fontSize);

            int GetFontSize() const { return fontSize; }

            void LoadFont(const char* path);
            std::map<char, FontCharacter> characterMap;
        private:
            int fontSize;
            FT_Face face;
    };

    void Text(std::string text, cg::Vec2f pos, unsigned int flag = 0u);
    cg::Vec2f MeasureText(std::string text);

    void PushFont(cg::Font& font);
    cg::Font* PopFont();
    cg::Font* GetCurrentFont();
}

#endif