#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

class Circle : public Shape {
public:
    int radius;

    Circle(int x, int y, int radius);
    void Draw(HDC hdc) const override;
};

#endif // CIRCLE_H
