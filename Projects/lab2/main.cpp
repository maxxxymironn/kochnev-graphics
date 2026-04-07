#include "figures.hpp"
#include <raylib.h>

struct Window {
    inline static float x;
    inline static float y;
    inline static float aspect;
};

void GetWindowInfo(float& wWidth, float& wHeight, float& windowAspect);
void DrawFigure(const Figure& figure, const float Sx, const float Sy, const float Ty);
void UpdateScalars(const Figure& figure, float& Sx, float& Sy, float& Ty, const float figureAspect, const bool keepAspect);

int main() {
    // Setting window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 400, "Kochnev Maxim 231 Lab2");
    SetTargetFPS(60);

    // Window info
    GetWindowInfo(Window::x, Window::y, Window::aspect);

    // New mode by press "M"
    bool keepAspect = false;
    // Change picture
    bool drawCandle = false;

    // figure info
    const Figure& figure = HARE;
    const float figureAspect = figure.Vx / figure.Vy;
    float Sx, Sy, Ty;
    UpdateScalars(figure, Sx, Sy, Ty, figureAspect, keepAspect);
    
    // figure2 info
    const Figure& figure2 = CANDLE;
    const float figureAspect2 = figure2.Vx / figure2.Vy;
    float Sx2, Sy2, Ty2;
    UpdateScalars(figure2, Sx2, Sy2, Ty2, figureAspect2, keepAspect);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (drawCandle) 
            DrawFigure(figure2, Sx2, Sy2, Ty2);
        else
            DrawFigure(figure, Sx, Sy, Ty);

        EndDrawing();

        if (IsWindowResized() || IsKeyPressed(KEY_M)) {
            if (IsKeyPressed(KEY_M))
                keepAspect = !keepAspect;

            GetWindowInfo(Window::x, Window::y, Window::aspect);

            UpdateScalars(figure, Sx, Sy, Ty, figureAspect, keepAspect);
            UpdateScalars(figure2, Sx2, Sy2, Ty2, figureAspect2, keepAspect);
        }

        if (IsKeyPressed(KEY_N))
            drawCandle = !drawCandle;
    }
    CloseWindow();

    return 0;
}

void GetWindowInfo(float& wWidth, float& wHeight, float& windowAspect) {
    wWidth = static_cast<float>(GetScreenWidth());
    wHeight = static_cast<float>(GetScreenHeight());

    windowAspect = wWidth / wHeight;
}

void DrawFigure(const Figure& figure, const float Sx, const float Sy, const float Ty) {
    for (size_t i = 0; i < figure.vertices.size(); i += 4) {
        DrawLineEx(
            {Sx * figure.vertices[i], Ty - Sy * figure.vertices[i + 1]},
            {Sx * figure.vertices[i + 2], Ty - Sy * figure.vertices[i + 3]},
            2,
            BLACK
        );
    }
}

void UpdateScalars(const Figure& figure, float& Sx, float& Sy, float& Ty, const float figureAspect, const bool keepAspect) {
    if (!keepAspect) {
        Sx = figureAspect < Window::aspect ? Window::y / figure.Vy : Window::x / figure.Vx;
        Sy = Sx;
    }
    else {
        Sx = Window::x / figure.Vx;
        Sy = Window::y / figure.Vy;
    }
    Ty = Sy * figure.Vy;
}