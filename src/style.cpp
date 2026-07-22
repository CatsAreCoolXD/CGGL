#include <stack>

#include "style.h"
#include "types.h"

#include <iostream>

namespace cg {
    namespace {
        std::stack<Style> styleStack;
    }

    Style GetStyle(cg::Color primaryColor, cg::Color secondaryColor, int quality, bool border, int borderThickness){
        Style style;
        style.primaryColor = primaryColor;
        style.secondaryColor = secondaryColor;
        style.quality = quality;
        style.border = border;
        style.borderThickness = borderThickness;
        return style;
    }

    void PushStyle(Style style){
        styleStack.push(style);
    }

    void PushPrimaryColor(cg::Color col){
        cg::Style currentStyle = cg::GetCurrentStyle();
        currentStyle.primaryColor = col;
        styleStack.push(currentStyle);
    }

    void PushSecondaryColor(cg::Color col){
        cg::Style currentStyle = cg::GetCurrentStyle();
        currentStyle.secondaryColor = col;
        styleStack.push(currentStyle);
    }

    void PushQuality(int quality){
        cg::Style currentStyle = cg::GetCurrentStyle();
        currentStyle.quality = quality;
        styleStack.push(currentStyle);
    }

    void PushBorder(bool border){
        cg::Style currentStyle = cg::GetCurrentStyle();
        currentStyle.border = border;
        styleStack.push(currentStyle);
    }

    void PushBorderThickness(int thickness){
        cg::Style currentStyle = cg::GetCurrentStyle();
        currentStyle.borderThickness = thickness;
        styleStack.push(currentStyle);
    }
    
    Style PopStyle(){
        if (styleStack.size() <= 1) {
            if (styleStack.empty())
                throw std::runtime_error("Stylestack is empty. Maybe you popped without pushing first?");
            else
                throw std::runtime_error("Only one style in stylestack. Maybe you popped without pushing first?");
        }

        Style style = styleStack.top();
        styleStack.pop();
        return style;
    }

    Style& GetCurrentStyle() {
        if (styleStack.empty())
            cg::PushStyle(GetStyle());

        return styleStack.top();
    }
}