#include <raylib.h>

const char* const LETTERS =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" \
    "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" \
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

int main() {
    // Setting window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(600, 480, "Kochnev Maxim 231 Lab1");
    SetTargetFPS(60);

    // Set custom font supporting UTF-8
    int letterCount = 0;
    int* codepoints = LoadCodepoints(LETTERS, &letterCount);
    Font font1 = LoadFontEx("Assets/Fonts/RUBIK-REGULAR.ttf", 100, codepoints, letterCount);

    // Window info
    const float windowHeight = static_cast<float>(GetScreenHeight());
    const float windowWidth = static_cast<float>(GetScreenWidth());

    const float heightRatio = windowHeight / 3.f;
    const float widthRatio = windowWidth / 3.f;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawLineEx({0, 0}, {windowWidth, windowHeight}, 6, RED);
        DrawLineEx({90, 50}, {windowWidth, 80}, 10, BLUE);

        DrawText("Welcome to computer graphics", 40, 350, 26, BLACK);
        DrawTextEx(font1, "Добро пожаловать на компьютерную графику", {40, 380}, 26, 0, BLACK);

        // Task
        DrawLineEx({0, heightRatio}, {widthRatio, windowHeight}, 5, GREEN);
        DrawLineEx({widthRatio, windowHeight}, {windowWidth, heightRatio * 2}, 5, GREEN);
        DrawLineEx({windowWidth, heightRatio * 2}, {widthRatio * 2, 0}, 5, GREEN);
        DrawLineEx({widthRatio * 2, 0}, {0, heightRatio}, 5, GREEN);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}