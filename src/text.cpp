#include "text.h"
#include "types.h"
#include "draw.h"
#include "shape.h"
#include "style.h"

#include <iostream>

namespace cg {
    namespace {
        FT_Library freeType;
        std::stack<cg::Font*> fontStack;
    }

    void InitiliazeFreeType(){
        if (FT_Init_FreeType(&freeType)){
            throw std::runtime_error("Failed to initialize FreeType library");
        }
    }

    Font::Font(const char* path, int fontSize) : fontSize(fontSize) {
        Font::LoadFont(path);
    }

    void Font::LoadFont(const char* path){
        if (FT_New_Face(freeType, path, 0, &face)){
            std::string msg = "Failed to load font with path " + (std::string)path + ". Ensure path is correct.";
            throw std::runtime_error(msg);
        }
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
        for (unsigned char c = 0; c < 128; c++){
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
                std::cout << "Failed to load glyph " << c << std::endl;
            }

            cg::Texture tex;
            tex.LoadGlyph(face);
            tex.FlipVertically();

            FontCharacter fontCharacter;
            fontCharacter.tex = tex;
            fontCharacter.advance = face->glyph->advance.x;
            fontCharacter.size = cg::Vec2f(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            fontCharacter.bearing = cg::Vec2f(face->glyph->bitmap_left, face->glyph->bitmap_top);

            characterMap.insert(std::pair<char, FontCharacter>(c, fontCharacter));
        }

        FT_Done_Face(face);
    }

    void PushFont(cg::Font& font){
        fontStack.push(&font);
    }

    cg::Font* PopFont(){
        if (fontStack.empty()) {
            std::cerr << "Fontstack is empty. Maybe you popped without pushing first?" << std::endl;
        }
        cg::Font* currentFont = fontStack.top();
        fontStack.pop();
        return currentFont;
    }

    cg::Font* GetCurrentFont(){
        return fontStack.top();
    }

    void Text(std::string text, cg::Vec2f pos, unsigned int flag){
        if (flag & FLAG_CENTERED) pos = pos - cg::MeasureText(text) / 2.f;
        cg::Font* font = fontStack.top();

        int i = 0;
        for (char& c : text){
            cg::FontCharacter& fontCharacter = font->characterMap[c];
            cg::Texture& tex = fontCharacter.tex;
            tex.SetTint(flag & FLAG_USE_SECONDARY_COLOR ? cg::GetCurrentStyle().secondaryColor : cg::GetCurrentStyle().primaryColor);
            int x = pos.x + fontCharacter.bearing.x;
            int y = pos.y - (fontCharacter.size.y - fontCharacter.bearing.y);
            tex.SetOrigin(cg::Vec2f(x, y));
            cg::Draw(tex, FLAG_NO_BORDER);

           pos.x += fontCharacter.advance >> 6; // Bitshift by 6 to divide by 64, since advance is in 1/64 pixels
        }
    }

    cg::Vec2f MeasureText(std::string text){
        cg::Font* font = fontStack.top();

        cg::Vec2f size;
        for (char& c : text){
            cg::FontCharacter& fontCharacter = font->characterMap[c];
            size.x += fontCharacter.size.x + ((fontCharacter.advance >> 6) - fontCharacter.size.x);
            size.y = std::max(size.y, (float)fontCharacter.size.y);
        }
        return size;
    }

    void ShutdownFreetype(){
        FT_Done_FreeType(freeType);
    }
}