#include "CGGL/gui.h"
#include "CGGL/shape.h"
#include "CGGL/style.h"
#include "CGGL/text.h"
#include "CGGL/input.h"

#include <iostream>
#include <unordered_map>

#define GUI_REQUIRE_SECTION(s) if(!currentSection) throw std::runtime_error((std::string)"You need to start a section before using " + s + "! Start a section with cg::GUIStartSection().");
#define GUI_CHECKBOX_OPTION_1

#define SPACING 5.f

namespace cg
{
    namespace {
        std::unordered_map<std::string, bool> guiElementsInformation;
    }

    int GetRoundedSize(cg::Vec2f dimensions){
        return std::max((dimensions.x + dimensions.y) / 100.f, 7.f);
    }

    /* GUI ELEMENTS */

    bool GUIInputBox(std::string name, std::string& text, cg::Vec2f pos, cg::Vec2f size, unsigned int flags){cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f delta = mousePos - pos;

        bool hovering = delta.x > 0 && delta.y > 0 && delta.x < size.x && delta.y < size.y;
        bool pressingButton = hovering && cg::IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        if (pressingButton) guiElementsInformation[name] = true;
        if ((!hovering && cg::IsMouseButtonDown(MOUSE_BUTTON_LEFT)) || cg::GetKeyDown(KEY_ENTER)) guiElementsInformation[name] = false;

        bool clicked = guiElementsInformation[name];

        if (clicked) {
            for (int key = KEY_FIRST; key < KEY_LAST_PRINTABLE; key++) {
                if (cg::GetKeyDown(key)) text += cg::KeyToChar(key);
            }
            if (cg::GetKeyDown(KEY_BACKSPACE) && !text.empty()) text.erase(text.end() - 1);
        }

        float factor = HOVER_COLOR_SCALE_FACTOR * (pressingButton ? HOVER_COLOR_SCALE_FACTOR : 1.f);
        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * cg::Color(factor, factor, factor, 1.f));

        cg::RoundedRectangle(pos, size, cg::GetRoundedSize(size), flags);
        cg::Text(text, cg::Vec2f(pos.x + SPACING, pos.y + size.y / 2  - cg::MeasureText(text).y / 2), FLAG_USE_SECONDARY_COLOR);

        if (hovering) cg::PopStyle();

        return cg::GetKeyDown(KEY_ENTER);
    }

    bool GUIButton(std::string text, cg::Vec2f pos, cg::Vec2f size, unsigned int flags){
        cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f delta = mousePos - pos;

        bool hovering = delta.x > 0 && delta.y > 0 && delta.x < size.x && delta.y < size.y;
        bool pressingButton = hovering && cg::IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        float factor = HOVER_COLOR_SCALE_FACTOR * (pressingButton ? HOVER_COLOR_SCALE_FACTOR : 1.f);
        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * cg::Color(factor, factor, factor, 1.f));
        
        cg::RoundedRectangle(pos, size, cg::GetRoundedSize(size), flags);
        cg::Text(text, pos + size / 2.f, FLAG_CENTERED | FLAG_USE_SECONDARY_COLOR);

        if (hovering) cg::PopStyle();

        return hovering && cg::GetMouseButtonDown(MOUSE_BUTTON_LEFT);
    }

    template <typename T>
    bool GUISlider(std::string text, cg::Vec2f pos, cg::Vec2f size, T& value, T min, T max, float scale) {
        if (guiElementsInformation[text] && cg::IsMouseButtonUp(MOUSE_BUTTON_LEFT))
            guiElementsInformation[text] = false;

        int fontSize = cg::GetCurrentFont()->GetFontSize();
        cg::Vec2f bounds = size + cg::Vec2f(cg::MeasureText(text).x + SPACING, 0.f);

        cg::RoundedRectangle(pos, cg::Vec2f(size.x, size.y), cg::GetRoundedSize(size));

        const float knobRadius = size.y / 2.f;
        cg::Vec2f mousePos(cg::GetMousePos());
        float knobPosX = ((float)value - (float)min) / ((float)max - (float)min);
        cg::Vec2f knobPos = pos + cg::Vec2f(knobPosX * size.x, size.y / 2.f);
        bool hovering = cg::Vec2f(mousePos - knobPos).Length() < knobRadius;
        bool draggingKnob = (hovering && cg::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) || guiElementsInformation[text];

        bool changedValue = false;
        if (draggingKnob) {
            float newValue = min + cg::clamp01((mousePos.x - pos.x) / size.x) * (max - min);
            knobPosX = ((float)value - (float)min) / ((float)max - (float)min);
            knobPos = pos + cg::Vec2f(knobPosX * size.x, size.y / 2.f);
            guiElementsInformation[text] = true;
            if (newValue != value){
                changedValue = true;
                value = newValue;
            }
        }
        
        cg::Text(text, pos + cg::Vec2f(size.x + SPACING, size.y / 2.f - cg::MeasureText(text).y / 2.f), FLAG_USE_SECONDARY_COLOR);

        cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * (hovering ? HOVER_COLOR_SCALE_FACTOR : 1.f) * (draggingKnob ? HOVER_COLOR_SCALE_FACTOR : 1.f));
        cg::Circle(knobPos, knobRadius, FLAG_CENTERED);
        cg::PopStyle();

        cg::Text(std::to_string(value), pos + size / 2.f, FLAG_CENTERED | FLAG_USE_SECONDARY_COLOR);

        return changedValue;
    }

    template bool cg::GUISlider<int>(std::string, cg::Vec2f, cg::Vec2f, int&, int, int, float);
    template bool cg::GUISlider<float>(std::string, cg::Vec2f, cg::Vec2f, float&, float, float, float);
    template bool cg::GUISlider<double>(std::string, cg::Vec2f, cg::Vec2f, double&, double, double, float);

    void GUICheckBox(std::string text, bool& value, cg::Vec2f pos, cg::Vec2f size){
        int fontSize = cg::GetCurrentFont()->GetFontSize();
        cg::Vec2f bounds = size + cg::Vec2f(cg::MeasureText(text).x + SPACING, 0.f);

        cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f delta = mousePos - pos;

        bool hovering = delta.x > 0 && delta.y > 0 && delta.x < size.x && delta.y < size.y;
        if (hovering && cg::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) value = !value;

        #ifdef GUI_CHECKBOX_OPTION_1

        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * HOVER_COLOR_SCALE_FACTOR);
        cg::RoundedRectangle(pos, size, cg::GetRoundedSize(size));
        if (value){
            cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * cg::Color(0, 3.f, 0));
            cg::RoundedRectangle(pos + size / 6.f, size - size / 3.f, cg::GetRoundedSize(size - size / 3.f));
            cg::PopStyle();
        }
        if (hovering) cg::PopStyle();

        #endif
        #ifdef GUI_CHECKBOX_OPTION_2

        cg::PushPrimaryColor(value ? cg::Color(0, 255, 0) : cg::Color(255, 0, 0));

        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * HOVER_COLOR_SCALE_FACTOR);
        cg::RoundedRectangle(pos, size, cg::GetRoundedSize(size));
        if (hovering) cg::PopStyle();
        cg::PopStyle();

        #endif

        cg::Text(text, pos + cg::Vec2f(size.x + SPACING, size.y / 2.f - cg::MeasureText(text).y / 2.f), FLAG_USE_SECONDARY_COLOR);
    }
}
