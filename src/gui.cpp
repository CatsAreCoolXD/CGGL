#include "CGGL/gui.h"
#include "CGGL/shape.h"
#include "CGGL/style.h"
#include "CGGL/text.h"
#include "CGGL/input.h"

#include <iostream>
#include <unordered_map>

#define GUI_REQUIRE_SECTION(s) if(!currentSection) throw std::runtime_error((std::string)"You need to start a section before using " + s + "! Start a section with cg::GUIStartSection().");
#define GUI_CHECKBOX_OPTION_1

namespace cg
{
    namespace {
        cg::GUISection* currentSection = nullptr;
        bool sameLine = false;

        std::unordered_map<std::string, bool> guiElementsInformation;
    }

    int GetRoundedSize(cg::Vec2f dimensions){
        return std::max((dimensions.x + dimensions.y) / 100.f, 7.f);
    }

    /* GUI SECTION */

    GUISection::GUISection(std::string title, cg::Vec2f pos, cg::Vec2f size){
        this->pos = pos;
        this->size = cg::Vec2f(std::max(size.x, 0.f), std::max(size.y, 0.f));

        currentPos = pos + cg::Vec2f(margin, size.y);

        cg::Color col = cg::GetBackgroundColor() * 1.5f;
        cg::PushPrimaryColor(col);
        cg::Rectangle(pos, size, FLAG_NO_BORDER);

        cg::PopStyle();

        int borderThickness = cg::GetCurrentStyle().borderThickness;
        cg::Vec2f dimensions = cg::MeasureText(title) * 1.2f;
        dimensions.y = 0.f;

        // Draw border
        cg::Vec2f bottomLeft(pos);
        cg::Vec2f topLeft(pos + cg::Vec2f(borderThickness, size.y));
        cg::Vec2f topRight(pos + size - cg::Vec2f(0.f, borderThickness));
        cg::Vec2f bottomRight(pos + cg::Vec2f(size.x - borderThickness, borderThickness));

        cg::Rectangle(bottomLeft, bottomRight - bottomLeft, FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);
        cg::Rectangle(topLeft, (topRight - topLeft) / 2.f - dimensions / 2.f, FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);
        cg::Rectangle(topRight, ((topRight - topLeft) / 2.f - dimensions / 2.f) * -1.f, FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);

        bottomRight = bottomRight - cg::Vec2f(0.f, borderThickness);

        cg::Rectangle(bottomLeft, topLeft - bottomLeft, FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);
        cg::Rectangle(bottomRight, topRight - bottomRight, FLAG_NO_BORDER | FLAG_USE_SECONDARY_COLOR);

        // Draw title text
        cg::Text(title, cg::Vec2f(topLeft.x + size.x / 2.f, topLeft.y), FLAG_CENTERED | FLAG_USE_SECONDARY_COLOR);

        currentPos.y -= cg::MeasureText(title).y / 2.f + spacing;
    }
    
    void GUISection::IncrementCurrentPosition(cg::Vec2f increment, bool& sameLine) {
        currentPos = cg::Vec2f(
            sameLine ? (currentPos.x + lastIncrement.x + spacing) : (pos.x + margin),
            sameLine ? currentPos.y : (currentPos.y - increment.y - spacing)
        );
        lastIncrement = increment;
        sameLine = false;
    }
    
    cg::Vec2f GUISection::GetCurrentPos() const {
        return currentPos;
    }

    void GUISection::SetSpacing(int spacing) {
        this->spacing = spacing;

        bool temp;
        GUISection::IncrementCurrentPosition(cg::Vec2f(), temp);
    }

    void GUISection::SetMargin(int margin){
        this->margin = margin;

        bool temp;
        GUISection::IncrementCurrentPosition(cg::Vec2f(), temp);
    }

    cg::GUISection* GUIStartSection(std::string title, cg::Vec2f pos, cg::Vec2f size){
        currentSection = new cg::GUISection(title, pos, size);
        return currentSection;
    }

    void GUIEndSection(){
        GUI_REQUIRE_SECTION("GUIEndSection()")
        if (currentSection) {
            delete currentSection;
            currentSection = nullptr;
        }
    }

    void GUISameLine(){
        GUI_REQUIRE_SECTION("GUISameLine()")

        sameLine = true;
    }

    void GUISetSpacing(int spacing){
        GUI_REQUIRE_SECTION("GUISetSpacing()")

        currentSection->SetSpacing(spacing);
    }

    void GUISetMargin(int margin){
        GUI_REQUIRE_SECTION("GUISetMargin()")

        currentSection->SetMargin(margin);
    }

    /* GUI ELEMENTS */

    void GUIText(std::string text, unsigned int flags){
        GUI_REQUIRE_SECTION("GUIText()")

        currentSection->IncrementCurrentPosition(cg::MeasureText(text), sameLine);

        cg::Vec2f pos = currentSection->GetCurrentPos();
        cg::Text(text, pos, flags & ~FLAG_CENTERED);
    }

    bool GUIInputBox(std::string name, std::string& text, unsigned int flags){
        GUI_REQUIRE_SECTION("GUIInputBox()")

        cg::Vec2f size = cg::Vec2f(std::min(std::max(200.f, cg::MeasureText(text).x + currentSection->GetSpacing()), currentSection->GetSize().x), 25);

        currentSection->IncrementCurrentPosition(size, sameLine);

        cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f buttonPos = currentSection->GetCurrentPos();
        cg::Vec2f delta = mousePos - buttonPos;

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

        cg::Vec2f pos = currentSection->GetCurrentPos();
        cg::RoundedRectangle(pos, size, cg::GetRoundedSize(size), flags);
        cg::Text(text, cg::Vec2f(pos.x + currentSection->GetSpacing(), pos.y + size.y / 2  - cg::MeasureText(text).y / 2), FLAG_USE_SECONDARY_COLOR);

        if (hovering) cg::PopStyle();

        return cg::GetKeyDown(KEY_ENTER);
    }

    bool GUIButton(std::string text, unsigned int flags){
        GUI_REQUIRE_SECTION("GUIButton()")

        int fontSize = cg::GetCurrentFont()->GetFontSize();
        cg::Vec2f buttonSize = cg::MeasureText(text) + cg::Vec2f(fontSize * 1.8f, fontSize);
        currentSection->IncrementCurrentPosition(buttonSize, sameLine);

        cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f buttonPos = currentSection->GetCurrentPos();
        cg::Vec2f delta = mousePos - buttonPos;

        bool hovering = delta.x > 0 && delta.y > 0 && delta.x < buttonSize.x && delta.y < buttonSize.y;
        bool pressingButton = hovering && cg::IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        float factor = HOVER_COLOR_SCALE_FACTOR * (pressingButton ? HOVER_COLOR_SCALE_FACTOR : 1.f);
        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * cg::Color(factor, factor, factor, 1.f));
        
        cg::RoundedRectangle(buttonPos, buttonSize, cg::GetRoundedSize(buttonSize), flags);
        cg::Text(text, buttonPos + buttonSize / 2.f, FLAG_CENTERED | FLAG_USE_SECONDARY_COLOR);

        if (hovering) cg::PopStyle();

        return hovering && cg::GetMouseButtonDown(MOUSE_BUTTON_LEFT);
    }

    template <typename T>
    bool GUISlider(std::string text, T& value, T min, T max, float scale) {
        GUI_REQUIRE_SECTION("GUISlider()")

        if (guiElementsInformation[text] && cg::IsMouseButtonUp(MOUSE_BUTTON_LEFT))
            guiElementsInformation[text] = false;

        int fontSize = cg::GetCurrentFont()->GetFontSize();
        cg::Vec2f size(fontSize * 10.f, fontSize * 2.f);
        cg::Vec2f bounds = size + cg::Vec2f(cg::MeasureText(text).x + currentSection->GetSpacing(), 0.f);

        currentSection->IncrementCurrentPosition(bounds, sameLine);
        cg::Vec2f pos = currentSection->GetCurrentPos();

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
        
        cg::Text(text, pos + cg::Vec2f(size.x + currentSection->GetSpacing(), size.y / 2.f - cg::MeasureText(text).y / 2.f), FLAG_USE_SECONDARY_COLOR);

        cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * (hovering ? HOVER_COLOR_SCALE_FACTOR : 1.f) * (draggingKnob ? HOVER_COLOR_SCALE_FACTOR : 1.f));
        cg::Circle(knobPos, knobRadius, FLAG_CENTERED);
        cg::PopStyle();

        cg::Text(std::to_string(value), pos + size / 2.f, FLAG_CENTERED | FLAG_USE_SECONDARY_COLOR);

        return changedValue;
    }

    template bool cg::GUISlider<int>(std::string, int&, int, int, float);
    template bool cg::GUISlider<float>(std::string, float&, float, float, float);
    template bool cg::GUISlider<double>(std::string, double&, double, double, float);

    void GUICheckBox(std::string text, bool& value){
        GUI_REQUIRE_SECTION("GUICheckBox()");

        int fontSize = cg::GetCurrentFont()->GetFontSize();
        cg::Vec2f checkBoxSize(fontSize * 2.f, fontSize * 2.f);
        cg::Vec2f bounds = checkBoxSize + cg::Vec2f(cg::MeasureText(text).x + currentSection->GetSpacing(), 0.f);

        currentSection->IncrementCurrentPosition(bounds, sameLine);

        cg::Vec2f pos = currentSection->GetCurrentPos();
        cg::Vec2f mousePos = cg::GetMousePos();
        cg::Vec2f delta = mousePos - pos;

        bool hovering = delta.x > 0 && delta.y > 0 && delta.x < checkBoxSize.x && delta.y < checkBoxSize.y;
        if (hovering && cg::GetMouseButtonDown(MOUSE_BUTTON_LEFT)) value = !value;

        #ifdef GUI_CHECKBOX_OPTION_1

        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * HOVER_COLOR_SCALE_FACTOR);
        cg::RoundedRectangle(pos, checkBoxSize, cg::GetRoundedSize(checkBoxSize));
        if (value){
            cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * cg::Color(0, 3.f, 0));
            cg::RoundedRectangle(pos + checkBoxSize / 6.f, checkBoxSize - checkBoxSize / 3.f, cg::GetRoundedSize(checkBoxSize - checkBoxSize / 3.f));
            cg::PopStyle();
        }
        if (hovering) cg::PopStyle();

        #endif
        #ifdef GUI_CHECKBOX_OPTION_2

        cg::PushPrimaryColor(value ? cg::Color(0, 255, 0) : cg::Color(255, 0, 0));

        if (hovering) cg::PushPrimaryColor(cg::GetCurrentStyle().primaryColor * HOVER_COLOR_SCALE_FACTOR);
        cg::RoundedRectangle(pos, checkBoxSize, cg::GetRoundedSize(checkBoxSize));
        if (hovering) cg::PopStyle();
        cg::PopStyle();

        #endif

        cg::Text(text, pos + cg::Vec2f(checkBoxSize.x + currentSection->GetSpacing(), checkBoxSize.y / 2.f - cg::MeasureText(text).y / 2.f), FLAG_USE_SECONDARY_COLOR);
    }
}
