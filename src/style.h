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

    Style GetStyle(cg::Color primaryColor = cg::Color(149, 147, 217), cg::Color secondaryColor = cg::Color(239, 246, 238), int quality = 16, bool border = true, int borderThickness = 2);

    void PushStyle(Style style);
    Style PopStyle();
    Style& GetCurrentStyle();
}

#endif