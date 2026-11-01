#ifndef CGGL_GUI_H
#define CGGL_GUI_H

#include "types.h"

#define HOVER_COLOR_SCALE_FACTOR 0.8f

namespace cg {
    // GUI Elements
    bool GUIButton(std::string text, cg::Vec2f pos, cg::Vec2f size, unsigned int flags = 0);

    template <typename T>
    bool GUISlider(std::string text, cg::Vec2f pos, cg::Vec2f size, T& value, T min = 0.f, T max = 0.f, float scale = 1.f);

    void GUICheckBox(std::string text, bool& value, cg::Vec2f pos, cg::Vec2f size);
    bool GUIInputBox(std::string name, std::string& text, cg::Vec2f pos, cg::Vec2f size, unsigned int flags = 0);
}

#endif