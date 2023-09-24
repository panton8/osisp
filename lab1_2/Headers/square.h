#ifndef SQUARE_H
#define SQUARE_H

#include "Shape.h"

class Square : public Shape {
public:
    int side_length;

    Square(int x, int y, int side_length);
    void Draw(HDC hdc) const override;
};

#endif // SQUARE_H
