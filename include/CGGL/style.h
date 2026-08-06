#ifndef DG_STYLE_H
#define DG_STYLE_H

#include "types.h"

namespace cg {
    struct Style {
        cg::Color primaryColor;
        cg::Color secondaryColor;
        bool border;
        int quality;
        int borderThickness;
    };

    void PushPrimaryColor(cg::Color col);
    void PushSecondaryColor(cg::Color col);
    void PushQuality(int quality);
    void PushBorder(bool border);
    void PushBorderThickness(int thickness);

    Style GetStyle(cg::Color primaryColor = cg::Color(25, 43, 194), cg::Color secondaryColor = cg::Color(253,240,213), int quality = 16, bool border = true, int borderThickness = 2);

    void PushStyle(Style style);
    Style PopStyle();
    Style& GetCurrentStyle();
}

#endif