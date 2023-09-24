#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <memory>
#include <vector>
#include <windows.h>
#include <algorithm>
#include "shape.h"
#include "circle.h"
#include "square.h"
#include "rectangle_shape.h"
#include "trajectory.h"

enum class ShapeType { Circle, Square, Rectangle };

void UpdateShapes(HWND hwnd);
void InvalidateShape(HWND hwnd, Shape *shape);
void OnKeyDown(HWND hwnd, WPARAM wParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int CalculateSize(int x, int y, int mouseX, int mouseY);
void MoveShape(Shape *shape, const Trajectory &trajectory);
void OnMouseButtonDown(HWND hWnd, int x, int y, WPARAM wParam);
Shape *SelectShapeAt(int x, int y);

#endif // UTILS_H
