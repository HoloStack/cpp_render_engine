#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    
    Vec3 normalize() const {
        float len = length();
        return len > 0 ? *this / len : Vec3(0, 0, 0);
    }
    
    float& operator[](int i) { return (&x)[i]; }
    const float& operator[](int i) const { return (&x)[i]; }
};

class Vec2 {
public:
    float x, y;
    
    Vec2() : x(0), y(0) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}
    
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
};

// Color class
class Color {
public:
    unsigned char r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255) 
        : r(r_), g(g_), b(b_), a(a_) {}
    
    Color operator*(float intensity) const {
        return Color(
            (unsigned char)(r * intensity),
            (unsigned char)(g * intensity),
            (unsigned char)(b * intensity),
            a
        );
    }
    
    Color operator+(const Color& c) const {
        return Color(
            std::min(255, (int)r + (int)c.r),
            std::min(255, (int)g + (int)c.g),
            std::min(255, (int)b + (int)c.b),
            a
        );
    }
};

#endif
