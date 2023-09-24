#include "square.h"

Square::Square(int x, int y, int side_length) : Shape(x, y), side_length(side_length) {}

void Square::Draw(HDC hdc) const {
    Rectangle(hdc, x - side_length / 2, y - side_length / 2, x + side_length / 2, y + side_length / 2);
}
