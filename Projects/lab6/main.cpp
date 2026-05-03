#include "namespaces.hpp"
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

void CheckPressedKeys(Mat4& T, const Mat4& initT);
void DrawFigure(const Mat4& T, const std::vector<my::Model>& models);
void DrawGrid();
bool isIgnorableLine(const std::string& line);
void OpenFile(Mat4& T, Mat4& initT, std::vector<my::Model>& models, bool& toDraw);
std::vector<my::Model> readFromFile(const char* fileName);

int main() {
    // Setting window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 600, "Kochnev Maxim 231 Lab6");
    SetWindowMinSize(
        static_cast<int>(Rect::left + Rect::right + 20.f),
        static_cast<int>(Rect::top + Rect::bottom + 30.f)
    );
    SetTargetFPS(60);

    Window::setInfo();
    Rect::setInfo();

    bool toDraw = false;
    bool toDrawGrid = false;

    // figures list description
    std::vector<my::Model> models;
    // matrixes for figure
    Mat4 T = Mat4(1.f); // accumulator matrix
    Mat4 initT;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        if (toDrawGrid)
            DrawGrid();
        if (toDraw)
            DrawFigure(T, models);
        DrawRectangleLinesEx({Rect::left, Rect::top, Rect::width, Rect::height}, 5, BLACK);
        if (GuiButton({Window::x - 90, 20, 80, 30}, "Open file"))
            OpenFile(T, initT, models, toDraw);
        EndDrawing();

        if (IsWindowResized()) {
            Window::setInfo();
            Rect::setInfo();
        }
        if (toDraw)
            CheckPressedKeys(T, initT);
        if (IsKeyPressed(KEY_V)) {
            toDrawGrid = !toDrawGrid;
        }
    }
    CloseWindow();

    return 0;
}

void CheckPressedKeys(Mat4& T, const Mat4& initT) {
    if (IsKeyPressed(KEY_C)) {
        T = initT;
    }

    if (IsKeyPressed(KEY_ONE)) {
        MyCamera::pType = projType::Ortho;
    } else if (IsKeyPressed(KEY_TWO)) {
        MyCamera::pType = projType::Frustum;
    } else if (IsKeyPressed(KEY_THREE)) {
        MyCamera::pType = projType::Perspective;
    }

    if (IsKeyPressed(KEY_W)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            T = lookAt(Vec3(0, 0, -0.1f), Vec3(0, 0, -1.1f), Vec3(0, 1, 0)) * T;
        } else {
            T = lookAt(Vec3(0, 0, -1), Vec3(0, 0, -2), Vec3(0, 1, 0)) * T;
        }
    } else if (IsKeyPressed(KEY_S)) {
        T = IsKeyDown(KEY_LEFT_SHIFT) ? lookAt(Vec3(0, 0, 0.1f), Vec3(0, 0, -0.9f), Vec3(0, 1, 0)) * T
                                      : lookAt(Vec3(0, 0, 1), Vec3(0, 0, 0), Vec3(0, 1, 0)) * T;
    }
    if (IsKeyPressed(KEY_A)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            T = lookAt(Vec3(-0.1f, 0, 0), Vec3(-0.1f, 0, -0.1f), Vec3(0, 1, 0)) * T;
        } else {
            T = lookAt(Vec3(-1, 0, 0), Vec3(-1, 0, -1), Vec3(0, 1, 0)) * T;
        }
    } else if (IsKeyPressed(KEY_D)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            T = lookAt(Vec3(0.1f, 0, 0), Vec3(0.1f, 0, -0.1f), Vec3(0, 1, 0)) * T;
        } else {
            T = lookAt(Vec3(1, 0, 0), Vec3(1, 0, -1), Vec3(0, 1, 0)) * T;
        }
    }

    if (IsKeyPressed(KEY_R)) {
        Vec3 u_new = Mat3(rotate(0.1f, Vec3(0, 0, 1))) * Vec3(0, 1, 0);
        T = lookAt(Vec3(0, 0, 0), Vec3(0, 0, -1), u_new) * T;
    } else if (IsKeyPressed(KEY_Y)) {
        Vec3 u_new = Mat3(rotate(-0.1f, Vec3(0, 0, 1))) * Vec3(0, 1, 0);
        T = lookAt(Vec3(0, 0, 0), Vec3(0, 0, -1), u_new) * T;
    }

    if (IsKeyPressed(KEY_T)) {          // rotate camera by OX
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            Mat4 M = rotateP(0.1f, Vec3(1, 0, 0), Vec3(0, 0, -MyCamera::dist));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 S_new = normalize(M * Vec4(0, 0, 0, 1));
            T = lookAt(S_new, Vec3(0, 0, -MyCamera::dist), u_new) * T;
        } else {
            Mat4 M = rotate(0.1f, Vec3(1, 0, 0));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 P_new = normalize(M * Vec4(0, 0, -1, 1));
            T = lookAt(Vec3(0, 0, 0), P_new, u_new) * T;
        }
    } else if (IsKeyPressed(KEY_G)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            Mat4 M = rotateP(-0.1f, Vec3(1, 0, 0), Vec3(0, 0, -MyCamera::dist));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 S_new = normalize(M * Vec4(0, 0, 0, 1));
            T = lookAt(S_new, Vec3(0, 0, -MyCamera::dist), u_new) * T;
        } else {
            Mat4 M = rotate(-0.1f, Vec3(1, 0, 0));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 P_new = normalize(M * Vec4(0, 0, -1, 1));
            T = lookAt(Vec3(0, 0, 0), P_new, u_new) * T;
        }
    }

    if (IsKeyPressed(KEY_F)) {          // rotate camera by OY 
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            Mat4 M = rotateP(0.1f, Vec3(0, 1, 0), Vec3(0, 0, -MyCamera::dist));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 S_new = normalize(M * Vec4(0, 0, 0, 1));
            T = lookAt(S_new, Vec3(0, 0, -MyCamera::dist), u_new) * T;
        } else {
            Mat4 M = rotate(0.1f, Vec3(0, 1, 0));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 P_new = normalize(M * Vec4(0, 0, -1, 1));
            T = lookAt(Vec3(0, 0, 0), P_new, u_new) * T;
        }
    } else if (IsKeyPressed(KEY_H)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            Mat4 M = rotateP(-0.1f, Vec3(0, 1, 0), Vec3(0, 0, -MyCamera::dist));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 S_new = normalize(M * Vec4(0, 0, 0, 1));
            T = lookAt(S_new, Vec3(0, 0, -MyCamera::dist), u_new) * T;
        } else {
            Mat4 M = rotate(-0.1f, Vec3(0, 1, 0));
            Vec3 u_new = Mat3(M) * Vec3(0, 1, 0);
            Vec3 P_new = normalize(M * Vec4(0, 0, -1, 1));
            T = lookAt(Vec3(0, 0, 0), P_new, u_new) * T;
        }
    }

    if (IsKeyPressed(KEY_I)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            --MyCamera::t;
        } else {
            ++MyCamera::t;
        }
    }

    if (IsKeyPressed(KEY_J)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            ++MyCamera::l;
        } else {
            --MyCamera::l;
        }
    }

    if (IsKeyPressed(KEY_K)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            ++MyCamera::b;
        } else {
            --MyCamera::b;
        }
    }

    if (IsKeyPressed(KEY_L)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            ++MyCamera::r;
        } else {
            --MyCamera::r;
        }
    }

    if (IsKeyPressed(KEY_U)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            if (MyCamera::n > 0.1f) {
                if (MyCamera::n - 0.2f > 0.1f) {
                    MyCamera::n -= 0.2f;
                } else {
                    MyCamera::n = 0.1f;
                }
            }
        } else if (MyCamera::n < MyCamera::f - 0.1f) {
            if (MyCamera::n + 0.2f < MyCamera::f - 0.1f) {
                MyCamera::n += 0.2f;
            } else {
                MyCamera::n = MyCamera::f - 0.1f;
            }
        }
    }

    if (IsKeyPressed(KEY_O)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            if (MyCamera::f - 0.2f > MyCamera::n + 0.1f) {
                MyCamera::f -= 0.2f;
            } else {
                MyCamera::f = MyCamera::n + 0.1f;
            }
        } else {
            MyCamera::f += 0.2f;
        }
    }

    if (IsKeyPressed(KEY_B)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            MyCamera::dist = (MyCamera::dist - 0.2f > 0.1f) ? MyCamera::dist - 0.2f 
                                                            : 0.1f;
        } else {
            MyCamera::dist += 0.2f;
        }
    }

    if (IsKeyPressed(KEY_Z)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            MyCamera::fovy_work = (MyCamera::fovy_work - 0.05f > 0.3f) ? MyCamera::fovy_work - 0.05f
                                                                      : 0.3f;
        } else {
            MyCamera::fovy_work = (MyCamera::fovy_work + 0.05f < 3.f) ? MyCamera::fovy_work + 0.05f
                                                                      : 3.f;
        }
    }

    if (IsKeyPressed(KEY_X)) {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            MyCamera::aspect_work = (MyCamera::aspect_work - 0.05f > 0.01f) ? MyCamera::aspect_work - 0.05f
                                                                      : 0.01f;
        } else {
            MyCamera::aspect_work += 0.05f;
        }
    }
}

void DrawFigure(const Mat4& T, const std::vector<my::Model>& models) {
    Mat4 proj;
    switch (MyCamera::pType) {
        case projType::Ortho: 
            proj = ortho(MyCamera::l, MyCamera::r, MyCamera::b, MyCamera::t, -MyCamera::n, -MyCamera::f); 
            break;
        case projType::Frustum:
            proj = frustum(MyCamera::l, MyCamera::r, MyCamera::b, MyCamera::t, MyCamera::n, MyCamera::f); 
            break;
        case projType::Perspective:
            proj = perspective(MyCamera::fovy_work, MyCamera::aspect_work, MyCamera::n, MyCamera::f); 
            break;
    }

    Mat3 cdr = cadrRL(
        Vec2(-1.f, -1.f), 
        Vec2(2.f, 2.f), 
        Vec2(Rect::width + Rect::left, Rect::top), 
        Vec2(Rect::width, Rect::height)
    );

    Mat4 C = proj * T;

    for (const auto model : models) {
        Mat4 TM = C * model.modelM;     // General model matrix
        for (const auto lines : model.figure) {
            Vec3 start_3D = normalize(TM * Vec4(lines.vertices[0], 1.f));
            Vec2 start = normalize(cdr * Vec3(Vec2(start_3D), 1.f));
            for (const auto line : lines.vertices) {
                Vec3 end_3D = normalize(TM * Vec4(line, 1.f));
                Vec2 end = normalize(cdr * Vec3(Vec2(end_3D), 1.f));
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

    DrawCircleV({Rect::cx, Rect::cy}, 3, BLACK);
}

bool isIgnorableLine(const std::string& line) {
    return line.find_first_not_of(" \t\r\n") == std::string::npos
        || line.front() == '#';
}

void OpenFile(Mat4& T, Mat4& initT, std::vector<my::Model>& models, bool& toDraw) {
    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = {{"Text files", "txt"}, {"All files", "*"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, nullptr);

    if (result == NFD_OKAY) {
        models = readFromFile(outPath);
        NFD_FreePath(outPath);

        MyCamera::initWorkPars(T);
        initT = T = lookAt(MyCamera::S, MyCamera::P, MyCamera::u);
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
    int r = 0, g = 0, b = 0;
    float thickness = 1.f;

    Mat4 M(1.f);    // accumulator mat for modelM
    Mat4 initM;
    std::stack<Mat4> transforms;
    std::vector<Path> figure;

    std::string line;
    while (in) {
        getline(in, line);
        if (isIgnorableLine(line))
            continue;

        std::stringstream s(line);
        std::string cmd;            // variable for command name
        s >> cmd;
        if (cmd == "camera") {
            s >> MyCamera::S.x >> MyCamera::S.y >> MyCamera::S.z;
            s >> MyCamera::P.x >> MyCamera::P.y >> MyCamera::P.z;
            s >> MyCamera::u.x >> MyCamera::u.y >> MyCamera::u.z;
        }
        else if (cmd == "screen") {
            s >> MyCamera::fovy_work >> MyCamera::aspect >> MyCamera::near >> MyCamera::far;
            MyCamera::fovy = MyCamera::fovy_work / 180.f * 3.1415f;  // translate from deegres to radians
        }
        else if (cmd == "color") {
            s >> r >> g >> b;
        }
        else if (cmd == "thickness") {
            s >> thickness;
        }
        else if (cmd == "path") {
            std::vector<Vec3> vertices;
            
            int n;
            s >> n;

            std::string extraStr;
            while (n > 0) {
                getline(in, extraStr);

                if (isIgnorableLine(extraStr))
                    continue;

                float x, y, z;
                std::stringstream extraS(extraStr);
                extraS >> x >> y >> z;
                vertices.push_back(Vec3(x, y, z));

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
        else if (cmd == "model") {      // new figure description
            figure.clear();

            Vec3 centerCoord;
            Vec3 size;
            s >> centerCoord.x >> centerCoord.y >> centerCoord.z >> size.x >> size.y >> size.z;
            
            float S = size.x / size.y < 1 ? 2.f / size.y 
                                          : 2.f / size.x;

            initM = scale(S, S, S) * translate(-centerCoord.x, -centerCoord.y, -centerCoord.z);
        }
        else if (cmd == "figure") {     // remember position of figure to draw it
            models.push_back(my::Model(figure, M * initM)); 
        }
        else if (cmd == "translate") {
            Vec3 T;
            s >> T.x >> T.y >> T.z;
            M = translate(T.x, T.y, T.z) * M;
        }
        else if (cmd == "scale") {
            float S;
            s >> S;
            M = scale(S, S, S) * M;
        }
        else if (cmd == "rotate") {
            float theta;
            Vec3 n;
            s >> theta >> n.x >> n.y >> n.z;
            M = rotate(theta / 180.f * 3.1415f, n) * M;
        }
        else if (cmd == "pushTransform") {  // push mat in stack
            transforms.push(M);
        }
        else if (cmd == "popTransform") {   // pop mat from stack
            M = transforms.top();
            transforms.pop();
        }
    }
    in.close();

    return models;
}