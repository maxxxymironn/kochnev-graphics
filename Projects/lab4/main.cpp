#include "clip.hpp"
#include "figures.hpp"
#include "transform.hpp"

#include <nfd.h>
#include <raygui.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

struct Window {
    inline static float x;
    inline static float y;

    inline static float cx;
    inline static float cy;

    inline static float aspect;
};

struct Rect {
    // distance to window border
    static constexpr float left = 30.f;
    static constexpr float right = 100.f;
    static constexpr float top = 20.f;
    static constexpr float bottom = 50.f;

    inline static float width;
    inline static float height;

    inline static float aspect;
    inline static float cx;
    inline static float cy;
};

void SetWindowInfo();
void SetRectInfo();
void DrawFigure(const Mat3& T, const std::vector<my::Model>& models);
void DrawGrid();

void OpenFile(Mat3& T, Mat3& initT, std::vector<my::Model>& models, bool& toDraw);
std::vector<my::Model> readFromFile(const char* fileName);
bool isIgnorableLine(const std::string& line);

void CheckPressedKeys(Mat3& T, const Mat3& initT);

int main() {
    // Setting window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 600, "Kochnev Maxim 231 Lab4");
    SetWindowMinSize(
        static_cast<int>(Rect::left + Rect::right + 20.f),
        static_cast<int>(Rect::top + Rect::bottom + 30.f)
    );
    SetTargetFPS(60);

    SetWindowInfo();
    SetRectInfo();

    bool toDraw = false;
    bool keepAspect = false;

    // figures list description
    std::vector<my::Model> models;
    // matrixes for figure
    Mat3 T = Mat3(1.f); // accumulator matrix
    Mat3 initT;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        if (toDraw)
            DrawFigure(T, models);
        DrawRectangleLinesEx({Rect::left, Rect::top, Rect::width, Rect::height}, 5, BLACK);
        DrawGrid();
        if (GuiButton({Window::x - 90, 20, 80, 30}, "Open file"))
            OpenFile(T, initT, models, toDraw);
        EndDrawing();

        if (IsWindowResized()) {
            SetWindowInfo();
            SetRectInfo();
        }
        if (toDraw)
            CheckPressedKeys(T, initT);
    }
    CloseWindow();

    return 0;
}

void SetWindowInfo() {
    Window::x = static_cast<float>(GetScreenWidth());
    Window::y = static_cast<float>(GetScreenHeight());

    Window::cx = Window::x / 2.f;
    Window::cy = Window::y / 2.f;

    Window::aspect = Window::x / Window::y;
}

void SetRectInfo() {
    Rect::width = Window::x - Rect::left - Rect::right;
    Rect::height = Window::y - Rect::top - Rect::bottom;

    Rect::aspect = Rect::width / Rect::height;
    Rect::cx = Rect::width / 2.f + Rect::left;
    Rect::cy = Rect::height / 2.f + Rect::top;
}

void DrawFigure(const Mat3& T, const std::vector<my::Model>& models) {
    for (const auto& model : models) {
        for (const auto& lines : model.figure) {
            Vec2 start = normalize(T * model.modelM * Vec3(lines.vertices[0], 1));
            for (const auto& line : lines.vertices) {
                Vec2 end = normalize(T * model.modelM * Vec3(line, 1));
                Vec2 checkEnd = end;

                if (clip(start, checkEnd, {Rect::left, Rect::top, Rect::width + Rect::left, Rect::height + Rect::top})) {
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
}

void DrawGrid() {
    float step = Rect::height / 20;
    float y = Rect::top;
    for (int i = 0; i < 20; ++i) {
        DrawLineEx(
            {Rect::left, y},
            {Rect::width + Rect::left, y},
            1,
            {0, 0, 0, 50}
        );
        y += step;
    }
    step = Rect::width / 40;
    float x = Rect::left;
    for (int j = 0; j < 40; ++j) {
        DrawLineEx(
            {x, Rect::top},
            {x, Rect::top + Rect::height},
            1,
            {0, 0, 0, 50}
        );
        x += step;
    }
}

void OpenFile(Mat3& T, Mat3& initT, std::vector<my::Model>& models, bool& toDraw) {
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = {{"Text files", "txt"}, {"All files", "*"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, nullptr);

    if (result == NFD_OKAY) {
        models = readFromFile(outPath);
        NFD_FreePath(outPath);

        float pictureAspect = my::Model::Vx / my::Model::Vy;
        float S = pictureAspect < Rect::aspect ? Rect::height / my::Model::Vy 
                                              : Rect::width / my::Model::Vx;

        Mat3 T1 = translate(-my::Model::Vx / 2, -my::Model::Vy / 2);
        Mat3 S1 = scale(S, -S);
        Mat3 T2 = translate(Rect::cx, Rect::cy);
        T = initT = T2 * (S1 * T1);

        toDraw = true;
    }
    else if (result == NFD_CANCEL)
        std::cerr << "INFO: NFD: user pressed cancel" << std::endl;
    else
        std::cerr << "ERROR: " << NFD_GetError() << std::endl;
}

std::vector<my::Model> readFromFile(const char* fileName) {
    std::ifstream in(fileName);

    std::vector<my::Model> models;
    int r, g, b;
    float thickness;

    Mat3 M(1.f);    // accumulator mat for modelM
    Mat3 initM;
    std::stack<Mat3> transforms;
    std::vector<Path> figure;

    std::string line;
    while (in) {
        getline(in, line);
        if (isIgnorableLine(line))
            continue;

        std::stringstream s(line);
        std::string cmd;            // variable for command name
        s >> cmd;
        if (cmd == "frame")
            s >> my::Model::Vx >> my::Model::Vy;
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

            figure.push_back(Path(
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
        else if (cmd == "model") { // new figure description
            figure.clear();

            Vec2 centerCoord;
            Vec2 size;
            s >> centerCoord.x >> centerCoord.y >> size.x >> size.y;
            
            float S = size.x / size.y < 1 ? 2.f / size.y 
                                          : 2.f / size.x;

            initM = scale(S) * translate(-centerCoord.x, -centerCoord.y);
        }
        else if (cmd == "figure") { // remember position of figure to draw it
            models.push_back(my::Model(figure, M * initM)); 
        }
        else if (cmd == "translate") {
            Vec2 T;
            s >> T.x >> T.y;
            M = translate(T.x, T.y) * M;
        }
        else if (cmd == "scale") {
            float S;
            s >> S;
            M = scale(S) * M;
        }
        else if (cmd == "rotate") {
            float theta;
            s >> theta;
            M = rotate(theta / 180.f * 3.1415f) * M;
        }
        else if (cmd == "pushTransform") { // push mat in stack
            transforms.push(M);
        }
        else if (cmd == "popTransform") { // pop mat from stack
            M = transforms.top();
            transforms.pop();
        }
    }
    in.close();

    return models;
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
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = IsKeyDown(KEY_Q) ? rotate(-0.01f) * T : rotate(0.01f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }

    // RY -- Rotate by 0.05rad
    if (IsKeyDown(KEY_R) || IsKeyDown(KEY_Y)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = IsKeyDown(KEY_R) ? rotate(-0.05f) * T : rotate(0.05f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
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
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(1.1f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }
    if (IsKeyDown(KEY_X)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(0.9f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }

    // UJ -- mirror by x/y
    if (IsKeyPressed(KEY_U)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = T * mirrorY();
        T = translate(Rect::cx, Rect::cy) * T;
    }
    if (IsKeyPressed(KEY_J)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = T * mirrorX();
        T = translate(Rect::cx, Rect::cy) * T;
    }
    // IK -- stretching/compression by x
    if (IsKeyDown(KEY_I)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(1.1f, 1.f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }
    if (IsKeyDown(KEY_K)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(0.9f, 1.f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }
    
    // OL -- stretching/compression by y
    if (IsKeyDown(KEY_O)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(1.f, 1.1f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }
    if (IsKeyDown(KEY_L)) {
        T = translate(-Rect::cx, -Rect::cy) * T;
        T = scale(1.f, 0.9f) * T;
        T = translate(Rect::cx, Rect::cy) * T;
    }
}