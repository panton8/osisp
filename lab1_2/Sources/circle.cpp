#include "circle.h"

Circle::Circle(int x, int y, int radius) : Shape(x, y), radius(radius) {}

void Circle::Draw(HDC hdc) const {
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
}
