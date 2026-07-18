#ifndef SHAPE_H
#define SHAPE_H

#include "types.h"

#define FLAG_CENTERED 1u
#define FLAG_NO_BORDER 2u
#define FLAG_USE_SECONDARY_COLOR 4u

#define TOP_RIGHT 0
#define BOTTOM_RIGHT 1
#define BOTTOM_LEFT 2
#define TOP_LEFT 3

namespace cg {
    // Square but never has a border
    void Quad(cg::Vec2f pos, int size, unsigned int flags = 0);
    void Square(cg::Vec2f pos, int size, unsigned int flags = 0);
    void UnfilledSquare(cg::Vec2f pos, int size, int thickness, unsigned int flags = 0);
    void RoundedSquare(cg::Vec2f pos, int size, int roundedSize, unsigned int flags = 0);
    void UnfilledRoundedSquare(cg::Vec2f pos, int size, int thickness, int roundedSize, unsigned int flags = 0);

    void Rectangle(cg::Vec2f pos, cg::Vec2f size, unsigned int flags = 0);
    void UnfilledRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, unsigned int flags = 0);

    void RoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int roundedSize, unsigned int flags = 0);
    void UnfilledRoundedRectangle(cg::Vec2f pos, cg::Vec2f size, int thickness, int roundedSize, unsigned int flags = 0);

    void Circle(cg::Vec2f pos, int radius, unsigned int flags = 0);
    void UnfilledCircle(cg::Vec2f pos, int radius, int thickness, unsigned int flags = 0);

    void SemiCircle(cg::Vec2f center, int size, int direction, unsigned int flags = 0);
    void UnfilledSemiCircle(cg::Vec2f center, int radius, int thickness, int direction, unsigned int flags);

    void Triangle(cg::Vertex* vertices);
    void Triangle3D(cg::Vertex* vertices);
}

#endif