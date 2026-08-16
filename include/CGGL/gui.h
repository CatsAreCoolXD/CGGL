#ifndef GUI_H
#define GUI_H

#include "types.h"

#define HOVER_COLOR_SCALE_FACTOR 0.8f

namespace cg {
    class GUISection {
        public:
            GUISection() {}
            GUISection(std::string title, cg::Vec2f pos, cg::Vec2f size);

            void IncrementCurrentPosition(cg::Vec2f increment, bool& sameLine);
            cg::Vec2f GetCurrentPos() const;
            int GetSpacing() const { return spacing; }
            cg::Vec2f GetSize() const { return size; }

            void SetSpacing(int spacing);
            void SetMargin(int margin);
        private:
            cg::Vec2f pos, size, currentPos, lastIncrement;
            int spacing = 10, margin = 10;
    };

    // Section
    cg::GUISection* GUIStartSection(std::string title = "", cg::Vec2f pos = cg::Vec2f(-1,-1), cg::Vec2f size = cg::Vec2f(-1,-1));
    void GUIEndSection();
    
    void GUISameLine();

    void GUISetSpacing(int spacing);
    void GUISetMargin(int margin);
    
    // GUI Elements
    void GUIText(std::string text, unsigned int flags = 0);
    bool GUIButton(std::string text, unsigned int flags = 0);

    template <typename T>
    bool GUISlider(std::string text, T& value, T min = 0.f, T max = 0.f, float scale = 1.f);

    void GUICheckBox(std::string text, bool& value);
    bool GUIInputBox(std::string name, std::string& text, unsigned int flags = 0);
}

#endif