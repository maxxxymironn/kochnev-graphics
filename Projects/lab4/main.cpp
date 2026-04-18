#include "figures.hpp"
#include "transform.hpp"
#include "clip.hpp"

#include <nfd.h>
#include <raygui.h>

#include <iostream>
#include <fstream>
#include <sstream>

struct Window {
    inline static float x;
    inline static float y;

    inline static float cx;
    inline static float cy;

    inline static float aspect;
};

void GetWindowInfo();
void DrawFigure(const Figure& figure, const Mat3& T, const Rectangle& border);
void CheckPressedKeys(Mat3& T, const Mat3& initT);

void OpenFile(Mat3& T, Mat3& initT, Figure& figure, const Rectangle& border, bool& toDraw);
Figure readFromFile(const char* fileName);
bool isIgnorableLine(const std::string& line);

int main() {
    // Setting window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 600, "Kochnev Maxim 231 Lab3");
    SetTargetFPS(60);
    GetWindowInfo();

    bool toDraw = false;

    // Figure drawing border info
    const Rectangle border = {50.f, 50.f, 300.f, 300.f};
    const Rectangle changedBorder = {
        border.x, 
        border.y, 
        border.width + border.x, 
        border.height + border.y
    };

    float aspectRect = border.width / border.height;

    // Figure info
    Figure figure;
    // matrixes for figure
    Mat3 T = Mat3(1.f); // accumulator matrix
    Mat3 initT;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (toDraw)
            DrawFigure(figure, T, changedBorder);
        
        DrawRectangleLinesEx(border, 5, BLACK);

        if (GuiButton({Window::x - 140, 20, 120, 30}, "Open file")) {
            OpenFile(T, initT, figure, border, toDraw);
        }

        EndDrawing();

        if (IsWindowResized())
            GetWindowInfo();

        if (toDraw)
            CheckPressedKeys(T, initT);
    }
    CloseWindow();

    return 0;
}

void GetWindowInfo() {
    Window::x = static_cast<float>(GetScreenWidth());
    Window::y = static_cast<float>(GetScreenHeight());

    Window::cx = Window::x / 2.f;
    Window::cy = Window::y / 2.f;

    Window::aspect = Window::x / Window::y;
}

void DrawFigure(const Figure& figure, const Mat3& T, const Rectangle& border) {
    for (const auto& lines : figure.paths) {
        Vec2 start = normalize(T * Vec3(lines.vertices[0], 1));

        for (const auto& line : lines.vertices) {
            Vec2 end = normalize(T * Vec3(line, 1));
            Vec2 checkEnd = end;
            
            if (clip(start, checkEnd, border)) {
                DrawLineEx(
                    {start.x, start.y},
                    {checkEnd.x, checkEnd.y},
                    lines.thickness,
                    lines.color
                );
            }
            
            start = end;
        }
    }
}

void OpenFile(Mat3& T, Mat3& initT, Figure& figure, const Rectangle& border, bool& toDraw) {
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = {{"Text files", "txt"}, {"All files", "*"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, nullptr);

    if (result == NFD_OKAY) {
        figure = readFromFile(outPath);
        NFD_FreePath(outPath);

        float figureAspect = figure.Vx / figure.Vy;
        float borderAspect = border.width / border.height;
        float S = figureAspect < borderAspect ? border.height / figure.Vy 
                                              : border.width / figure.Vx;

        Mat3 T1 = translate(-figure.Vx / 2, -figure.Vy / 2);
        Mat3 S1 = scale(S, -S);
        Mat3 T2 = translate(border.width / 2 + border.x, border.height / 2 + border.y);
        T = initT = T2 * (S1 * T1);

        toDraw = true;
    }
    else if (result == NFD_CANCEL)
        std::cerr << "INFO: NFD: user pressed cancel" << std::endl;
    else
        std::cerr << "ERROR: " << NFD_GetError() << std::endl;
}

Figure readFromFile(const char* fileName) {
    std::ifstream in(fileName);

    Figure figure;
    int r, g, b;
    float thickness;

    std::string line;
    while (in) {
        getline(in, line);
        if (isIgnorableLine(line))
            continue;

        std::stringstream s(line);
        std::string cmd;            // Переменная для имени команды
        s >> cmd;

        if (cmd == "frame") {
            s >> figure.Vx >> figure.Vy;
        }
        else if (cmd == "color")
            s >> r >> g >> b;
        else if (cmd == "thickness")
            s >> thickness;
        else if (cmd == "path") {
            std::vector<Vec2> vertices;
            
            int n;
            s >> n;

            std::string extraStr;
            while (n > 0) {
                getline(in, extraStr);

                if (isIgnorableLine(extraStr))
                    continue;

                float x, y;
                std::stringstream extraS(extraStr);
                extraS >> x >> y;
                vertices.push_back(Vec2(x, y));

                --n;
            }

            figure.paths.push_back(Path(
                vertices,
                Color{
                    static_cast<uint8_t>(r),
                    static_cast<uint8_t>(g),
                    static_cast<uint8_t>(b),
                    255
                },
                thickness
            ));
        }
    }
    in.close();

    return figure;
}

bool isIgnorableLine(const std::string& line) {
    return line.find_first_not_of(" \t\r\n") == std::string::npos
        || line.front() == '#';
}

void CheckPressedKeys(Mat3& T, const Mat3& initT) {
    if (IsKeyPressed(KEY_C))
        T = initT;

    // QE -- Rotate by 0.01rad
    if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_E)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = IsKeyDown(KEY_Q) ? rotate(-0.01f) * T : rotate(0.01f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }

    // RY -- Rotate by 0.05rad
    if (IsKeyDown(KEY_R) || IsKeyDown(KEY_Y)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = IsKeyDown(KEY_R) ? rotate(-0.05f) * T : rotate(0.05f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }

    // WASD -- shift by 1 pixel
    if (IsKeyDown(KEY_W)) 
        T = translate(0, -1) * T;
    if (IsKeyDown(KEY_S))
        T = translate(0, 1) * T;
    if (IsKeyDown(KEY_A))
        T = translate(-1, 0) * T;
    if (IsKeyDown(KEY_D))
        T = translate(1, 0) * T;
        
    // TGFH -- shift for 10 pixels
    if (IsKeyDown(KEY_T)) 
        T = translate(0, -10) * T;
    if (IsKeyDown(KEY_G))
        T = translate(0, 10) * T;
    if (IsKeyDown(KEY_F))
        T = translate(-10, 0) * T;
    if (IsKeyDown(KEY_H))
        T = translate(10, 0) * T;

    // ZX -- increase/decrease by 1.1
    if (IsKeyDown(KEY_Z)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(1.1f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }
    if (IsKeyDown(KEY_X)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(0.9f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }

    // UJ -- mirror by x/y
    if (IsKeyPressed(KEY_U))
        T = T * mirrorY();
    if (IsKeyPressed(KEY_J))
        T = T * mirrorX();

    // IK -- stretching/compression by x
    if (IsKeyDown(KEY_I)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(1.1f, 1.f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }
    if (IsKeyDown(KEY_K)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(0.9f, 1.f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }
    
    // OL -- stretching/compression by y
    if (IsKeyDown(KEY_O)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(1.f, 1.1f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }
    if (IsKeyDown(KEY_L)) {
        T = translate(-Window::cx, -Window::cy) * T;
        T = scale(1.f, 0.9f) * T;
        T = translate(Window::cx, Window::cy) * T;
    }
}