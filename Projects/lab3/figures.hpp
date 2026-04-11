#pragma once
#include "matrix.hpp"
#include <raylib.h>
#include <vector>

struct Path {
    std::vector<Vec2> vertices;
    Color color;
    float thickness;

    Path(std::vector<Vec2> vertices, Color color, float thickness)
        : vertices(vertices)
        , color(color)
        , thickness(thickness) {}
};

struct Figure {
    std::vector<Path> paths;
    float Vx;
    float Vy;
    
    Figure() = default;
    Figure(std::vector<Path> paths, float Vx, float Vy)
        : paths(paths)
        , Vx(Vx)
        , Vy(Vy) {}
};