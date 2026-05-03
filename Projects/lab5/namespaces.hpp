#pragma once

#include "matrix.hpp"
#include <raylib.h>

enum class projType {
    Ortho,
    Frustum,
    Perspective
};

namespace Window {
    float x;
    float y;
    float cx;
    float cy;
    float aspect;

    void setInfo() {
        x = static_cast<float>(GetScreenWidth());
        y = static_cast<float>(GetScreenHeight());

        cx = x / 2.f;
        cy = y / 2.f;

        aspect = x / y;
    }
}

namespace Rect {
    // distance to window borders
    float left = 30.f;
    float right = 100.f;
    float top = 20.f;
    float bottom = 50.f;

    float width;
    float height;
    float aspect;
    float cx;
    float cy;

    void setInfo() {
        width = Window::x - left - right;
        height = Window::y - top - bottom;

        aspect = width / height;
        cx = width / 2.f + left;
        cy = height / 2.f + top;
    }
}

namespace MyCamera {
    /* Camera info */ 
    Vec3 S;         // point of camera
    Vec3 P;         // point camera look at
    Vec3 u;         // vector up
    float dist;     // distance between S - P

    /* Projection info */
    float fovy, aspect;             // view angle and camera aspect
    float fovy_work, aspect_work;   // variables for fovy and aspect
    float near, far;                // distnace to camera and to horizon
    float n, f;                     // variables for near and far
    float l, r, t, b;               // helping variables
    projType pType = projType::Ortho;

    void initWorkPars(Mat4& T) {
        n = near;
        f = far;
        fovy_work = fovy;
        aspect_work = aspect;

        float Vy = 2 * near * tan(fovy / 2);
        float Vx = aspect * Vy;
        l = -Vx / 2;
        r = Vx / 2;
        b = -Vy / 2;
        t = Vy / 2;

        dist = length(P - S);
    }
}