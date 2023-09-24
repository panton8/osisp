#include "rectangle_shape.h"

RectangleShape::RectangleShape(int x, int y, int width, int height) : Shape(x, y), width(width), height(height) {}

void RectangleShape::Draw(HDC hdc) const {
    Rectangle(hdc, x - width / 2, y - height / 4, x + width / 2, y + height / 4);
}
