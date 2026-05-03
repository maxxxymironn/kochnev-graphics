#pragma once
#include <cmath>

struct Vec3;
struct Vec4;

struct Vec2 {
    float x = 0;
    float y = 0;

    Vec2() {}
    Vec2(float a, float b) : x(a), y(b) {}
    Vec2(Vec3 v);
};

struct Vec3 {
    float x = 0;
    float y = 0;
    float z = 0;

    Vec3() {}
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
    Vec3(const Vec2& v, float c) : Vec3(v.x, v.y, c) {}
    Vec3(Vec4 v);

    Vec3& operator*=(const Vec3& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    const Vec3 operator*(const Vec3& v) const {
        return Vec3(*this) *= v;
    }

    Vec3& operator*=(const float& n) {
        (*this) *= Vec3(n, n, n);
        return *this;
    }

    const Vec3 operator*(const float& n) {
        return Vec3(*this) *= n;
    }

    Vec3 operator+=(const Vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    
    const Vec3 operator+(const Vec3& v) const {
        return Vec3(*this) += v;
    }

    Vec3 operator-=(const Vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    const Vec3 operator-(const Vec3& v) const {
        return Vec3(*this) -= v;
    }
    
    float& operator[](size_t i) {
        return (reinterpret_cast<float*>(this))[i];
    }

    const float& operator[](size_t i) const {
        return (reinterpret_cast<const float*>(this))[i];
    }
};

struct Vec4 {
    float x, y, z, a;

    Vec4() {}
    Vec4(float a, float b, float c, float d) : x(a), y(b), z(c), a(d) {}
    Vec4(Vec3 v, float c) : Vec4(v.x, v.y, v.z, c) {}

    Vec4& operator*=(const Vec4& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        a *= v.a;

        return *this;
    }

    const Vec4 operator*(const Vec4& v) const {
        return Vec4(*this) *= v;
    }

    float& operator[](size_t i) {
        return (reinterpret_cast<float*>(this))[i];
    }

    const float& operator[](size_t i) const {
        return (reinterpret_cast<const float*>(this))[i];
    }
};

Vec2::Vec2(Vec3 v) : x(v.x), y(v.y) {}

Vec3::Vec3(Vec4 v) : x(v.x), y(v.y), z(v.z) {}

inline float dot(const Vec3& a, const Vec3& b) {
    Vec3 tmp = a * b;
    return tmp.x + tmp.y + tmp.z;
}

inline float dot(const Vec4& a, const Vec4& b) {
    Vec4 tmp = a * b;
    return tmp.x + tmp.y + tmp.z + tmp.a;   
}

inline Vec2 normalize(const Vec3& v) {
    return Vec2(v.x / v.z, v.y / v.z);
}

inline Vec3 normalize(const Vec4& v) {
    return Vec3(v.x / v.a, v.y / v.a, v.z / v.a);
}

float length(Vec3 p) {
    return sqrtf(dot(p, p));
}

Vec3 norm(Vec3 p) {     // Vector p normalazing 
    return normalize(Vec4(p, length(p)));
}

struct Mat4 {
    Vec4 row1, row2, row3, row4;

    Mat4() {};
    Mat4(const Vec4& r1, const Vec4& r2, const Vec4& r3, const Vec4& r4) 
        : row1(r1), row2(r2), row3(r3), row4(r4) {}
    Mat4(float a) {
        row1 = Vec4(a, 0.f, 0.f, 0.f);
        row2 = Vec4(0.f, a, 0.f, 0.f);
        row3 = Vec4(0.f, 0.f, a, 0.f);
        row4 = Vec4(0.f, 0.f, 0.f, a);
    }

    Vec4& operator[](size_t i) {
        return (reinterpret_cast<Vec4*>(this))[i];
    }

    const Vec4& operator[](size_t i) const {
        return (reinterpret_cast<const Vec4*>(this))[i];
    }

    Mat4& transpose() {
        Mat4 tmp(*this);
        
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j)
                (*this)[i][j] = tmp[j][i];
        }

        return *this;
    }

    const Vec4 operator*(const Vec4& v) const {
        Vec4 res{};
        
        for (int i = 0; i < 4; ++i)
            res[i] = dot((*this)[i], v);

        return res;
    }

    Mat4& operator*=(const Mat4& m) {
        Mat4 A(*this);
        Mat4 B(m);
        B.transpose();

        for (int i = 0; i < 4; ++i) {
            (*this)[i] = A * B[i];
        }

        return this->transpose();
    }

    const Mat4 operator*(const Mat4& m) const {
        return Mat4(*this) *= m;
    }
};

struct Mat3 {
    Vec3 row1{};
    Vec3 row2{};
    Vec3 row3{};

    Mat3() {};
    
    Mat3(const Vec3& r1, const Vec3& r2, const Vec3& r3) : row1(r1), row2(r2), row3(r3) {}
    
    Mat3(float a) {
        row1 = Vec3(a, 0.f, 0.f);
        row2 = Vec3(0.f, a, 0.f);
        row3 = Vec3(0.f, 0.f, a);
    }
    
    Mat3(const Mat4& m) {
        row1 = Vec3(m.row1.x, m.row1.y, m.row1.z);
        row2 = Vec3(m.row2.x, m.row2.y, m.row2.z);
        row3 = Vec3(m.row3.x, m.row3.y, m.row3.z);
    }

    Vec3& operator[](size_t i) {
        return (reinterpret_cast<Vec3*>(this))[i];
    }

    const Vec3& operator[](size_t i) const {
        return (reinterpret_cast<const Vec3*>(this))[i];
    }

    Mat3& transpose() {
        Mat3 tmp(*this);
        
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j)
                (*this)[i][j] = tmp[j][i];
        }

        return *this;
    }

    const Vec3 operator*(const Vec3& v) const {
        Vec3 res{};
        
        for (int i = 0; i < 3; ++i)
            res[i] = dot((*this)[i], v);

        return res;
    }

    Mat3& operator+=(const Mat3& m) {
        Mat3 B(m);
        for (int i = 0; i < 3; ++i) {
            (*this)[i] += B[i];
        }
        return *this;
    }

    const Mat3 operator+(const Mat3& m) {
        return Mat3(*this) += m;
    }

    Mat3& operator*=(const float& n) {
        for (int i = 0; i < 3; ++i) {
            (*this)[i] *= n;
        }
        return *this;
    }

    const Mat3 operator*(const float& n) {
        return Mat3(*this) *= n;
    }

    Mat3& operator*=(const Mat3& m) {
        Mat3 A(*this);
        Mat3 B(m);
        B.transpose();

        for (int i = 0; i < 3; ++i) {
            (*this)[i] = A * B[i];
        }

        return this->transpose();
    }

    const Mat3 operator*(const Mat3& m) const {
        return Mat3(*this) *= m;
    }
};

Mat3 crossM(Vec3 p) {   // prepares matrix for cross product of two vectors
    return Mat3(
        Vec3(0.f, -p.z, p.y),
        Vec3(p.z, 0.f, -p.x),
        Vec3(-p.y, p.x, 0.f)
    );
}

Vec3 cross(Vec3 p, Vec3 q) {
    return crossM(p) * q;
}
