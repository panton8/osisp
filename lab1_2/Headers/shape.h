#ifndef SHAPE_H
#define SHAPE_H

#include <windows.h>

class Shape {
public:
    int x, y;
    Shape(int x, int y);
    virtual ~Shape() = default;
    virtual void Draw(HDC hdc) const = 0;
};
#endif // SHAPE_H
