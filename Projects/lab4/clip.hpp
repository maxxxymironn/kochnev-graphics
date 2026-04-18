#pragma once
#include "matrix.hpp"
#include <raylib.h>
#include <algorithm>

void swap(Vec2& a, Vec2& b) {
    Vec2 temp = a;

    a = b;
    b = temp;
}

unsigned int codeKS(const Vec2& point, const Rectangle& border) {
    unsigned int code = 0;

    if (point.x < border.x)
        code += 1;
    else if (point.x > border.width)
        code += 2;
    if (point.y < border.y)
        code += 4;
    else if (point.y > border.height)
        code += 8;

    return code;
}

bool clip(Vec2& A, Vec2& B, const Rectangle& border) {
    unsigned int codeA = codeKS(A, border);
    unsigned int codeB = codeKS(B, border);

    while (codeA | codeB) {
        if (codeA & codeB)      // line isn't in border
            return false;

        if (codeA == 0) {       // if A isn't in border
            swap(A, B);
            std::swap(codeA, codeB);
        }

        if (codeA & 1) {        // point is left the border
            A.y = A.y + (B.y - A.y) * (border.x - A.x) / (B.x - A.x);
            A.x = border.x;
        }
        else if (codeA & 2) {   // point is right the border
            A.y = A.y + (B.y - A.y) * (border.width - A.x) / (B.x - A.x);
            A.x = border.width;
        }
        else if (codeA & 4) {   // point is above the border
            A.x = A.x + (B.x - A.x) * (border.y - A.y) / (B.y - A.y);
            A.y = border.y;
        }
        else {                  // point is below the border
            A.x = A.x + (B.x - A.x) * (border.height - A.y) / (B.y - A.y);
            A.y = border.height;
        }

        codeA = codeKS(A, border);
    }
    return true;
}