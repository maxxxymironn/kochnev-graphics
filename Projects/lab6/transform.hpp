#pragma once

#include "matrix.hpp"
#include <cmath>

inline Mat3 translate(float Tx, float Ty) {
    Mat3 res = Mat3(1.f);
    res[0][2] = Tx;
    res[1][2] = Ty;

    return res;
}

inline Mat4 translate(float Tx, float Ty, float Tz) {
    Mat4 res = Mat4(1.f);
    res[0][3] = Tx;
    res[1][3] = Ty;
    res[2][3] = Tz;

    return res;
}

inline Mat3 scale(float Sx, float Sy) {
    Mat3 res = Mat3(1.f);
    res[0][0] = Sx;
    res[1][1] = Sy;

    return res;
}

inline Mat3 scale(float S) {
    return scale(S, S);
}

inline Mat4 scale(float Sx, float Sy, float Sz) {
    Mat4 res = Mat4(1.f);
    res[0][0] = Sx;
    res[1][1] = Sy;
    res[2][2] = Sz;

    return res;
}

inline Mat3 rotate(float theta) {
    Mat3 res = Mat3(1.f);
    res[0][0] = static_cast<float>(cos(theta));
    res[1][1] = res[0][0];
    res[0][1] = static_cast<float>(-sin(theta));
    res[1][0] = -res[0][1];

    return res;
}

inline Mat4 rotate(float theta, Vec3 n) {
    Mat3 N = crossM(norm(n));
    Mat3 leftPart = N * sin(theta);
    Mat3 rightPart = (N * N) * (1 - cos(theta));
    Mat3 preRes = Mat3(1.f) + (leftPart + rightPart);

    return Mat4(
        Vec4(preRes.row1, 0),
        Vec4(preRes.row2, 0),
        Vec4(preRes.row3, 0),
        Vec4(0, 0, 0, 1)
    );
}

// вращение вектора P относительно прямой n
inline Mat4 rotateP(float theta, Vec3 n, Vec3 P) {
    return translate(P.x, P.y, P.z) * (rotate(theta, n) * translate(-P.x, -P.y, -P.z));
}

inline Mat3 mirrorY() {
    Mat3 res = Mat3(1.f);
    res[0][0] = -1.f;
    return res;
}

inline Mat3 mirrorX() {
    Mat3 res = Mat3(1.f);
    res[1][1] = -1.f;
    return res;
}

inline Mat4 lookAt(Vec3 S, Vec3 P, Vec3 u) {
    // 1) translate begin of coords to S (point of View)
    Mat4 T = translate(-S.x, -S.y, -S.z);
    // 2) поиск направляющих векторов для осей системы коорд наблюдателя в мировой системе
    Vec3 e3 = norm(S - P);
    Vec3 e1 = norm(cross(u, e3));
    Vec3 e2 = norm(cross(e3, e1));
    
    Mat4 R = Mat4(
        Vec4(e1, 0),
        Vec4(e2, 0),
        Vec4(e3, 0),
        Vec4(0, 0, 0, 1)
    );

    return R * T;
}

inline Mat4 ortho(float l, float r, float b, float t, float zn, float zf) {
    return Mat4(
        Vec4(2.f / (r - l), 0.f, 0.f, -(r + l) / (r - l)),
        Vec4(0.f, 2.f / (t - b), 0.f, -(t + b) / (t - b)),
        Vec4(0.f, 0.f, -2.f / (zf - zn), -(zf + zn) / (zf - zn)),
        Vec4(0.f, 0.f, 0.f, 1.f)
    );
}

inline Mat4 frustum(float l, float r, float b, float t, float n, float f) {
    return Mat4(
        Vec4(2.f * n / (r - l), 0.f, (r + l) / (r - l), 0.f),
        Vec4(0.f, 2.f * n / (t - b), (t + b) / (t - b), 0.f),
        Vec4(0.f, 0.f, -(f + n) / (f - n), -2.f * f * n / (f - n)),
        Vec4(0.f, 0.f, -1.f, 0.f)
    );
}

inline Mat4 perspective(float fovy, float aspect, float n, float f) {
    return Mat4(
        Vec4(1 / (tanf(fovy / 2.f) * aspect), 0.f, 0.f, 0.f),
        Vec4(0.f, 1 / tanf(fovy / 2.f), 0.f, 0.f),
        Vec4(0.f, 0.f, -(f = n) / (f - n), -2 * f * n / (f - n)),
        Vec4(0.f, 0.f, -1.f, 0.f)
    );
}

inline Mat3 cadrRL(Vec2 Vc, Vec2 V, Vec2 Wc, Vec2 W) {
    return translate(Wc.x, Wc.y) *
            (scale(W.x / V.x, -W.y / V.y) * translate(Vc.x, Vc.y));
}