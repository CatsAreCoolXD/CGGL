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

            void LoadFont(const char* path, int fontSize);
            std::map<char, FontCharacter> characterMap;
        private:
            FT_Face face;
    };

    void Text(std::string text, cg::Vec2f pos);

    void PushFont(cg::Font font);
    cg::Font PopFont();
    cg::Font GetCurrentFont();
}

#endif