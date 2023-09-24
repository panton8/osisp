#ifndef RECTANGLESHAPE_H
#define RECTANGLESHAPE_H

#include "Shape.h"

class RectangleShape : public Shape {
public:
    int width, height;

    RectangleShape(int x, int y, int width, int height);
    void Draw(HDC hdc) const override;
};

#endif // RECTANGLESHAPE_H
