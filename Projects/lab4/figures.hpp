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

struct Picture {
    std::vector<Path> paths;
    float Vx;
    float Vy;
    
    Picture() = default;
    Picture(std::vector<Path> paths, float Vx, float Vy)
        : paths(paths)
        , Vx(Vx)
        , Vy(Vy) {}
};

namespace my {
    struct Model {
        inline static float Vx;
        inline static float Vy;
        std::vector<Path> figure; // Picture members
        Mat3 modelM;
    
        Model(std::vector<Path> fig, Mat3 model) : figure(fig), modelM(model) {}
    };
}