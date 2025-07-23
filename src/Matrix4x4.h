#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "Vec3.h"
#include <cmath>

class Matrix4x4 {
public:
    float m[4][4];

    Matrix4x4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Vec3 transform(const Vec3& v, float w = 1.0f) const {
        float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * w;
        float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * w;
        float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * w;
        float wResult = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * w;

        if (wResult != 0.0f && wResult != 1.0f) {
            return Vec3(x / wResult, y / wResult, z / wResult);
        }
        return Vec3(x, y, z);
    }

    static Matrix4x4 translation(float x, float y, float z) {
        Matrix4x4 result;
        result.m[0][3] = x;
        result.m[1][3] = y;
        result.m[2][3] = z;
        return result;
    }

    static Matrix4x4 rotationX(float angle) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[1][1] = c;
        result.m[1][2] = -s;
        result.m[2][1] = s;
        result.m[2][2] = c;
        return result;
    }

    static Matrix4x4 rotationY(float angle) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[0][0] = c;
        result.m[0][2] = s;
        result.m[2][0] = -s;
        result.m[2][2] = c;
        return result;
    }

    static Matrix4x4 rotationZ(float angle) {
        Matrix4x4 result;
        float c = std::cos(angle);
        float s = std::sin(angle);
        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;
        return result;
    }

    static Matrix4x4 scaling(float x, float y, float z) {
        Matrix4x4 result;
        result.m[0][0] = x;
        result.m[1][1] = y;
        result.m[2][2] = z;
        return result;
    }

    static Matrix4x4 perspective(float fov, float aspect, float near, float far) {
        Matrix4x4 result;
        float f = 1.0f / std::tan(fov * 0.5f);
        result.m[0][0] = f / aspect;
        result.m[1][1] = f;
        result.m[2][2] = (far + near) / (near - far);
        result.m[2][3] = (2.0f * far * near) / (near - far);
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;
        return result;
    }

    static Matrix4x4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 forward = (target - eye).normalize();
        Vec3 right = forward.cross(up).normalize();
        Vec3 newUp = right.cross(forward);

        Matrix4x4 result;
        result.m[0][0] = right.x;
        result.m[0][1] = right.y;
        result.m[0][2] = right.z;
        result.m[0][3] = -right.dot(eye);

        result.m[1][0] = newUp.x;
        result.m[1][1] = newUp.y;
        result.m[1][2] = newUp.z;
        result.m[1][3] = -newUp.dot(eye);

        result.m[2][0] = -forward.x;
        result.m[2][1] = -forward.y;
        result.m[2][2] = -forward.z;
        result.m[2][3] = forward.dot(eye);

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;

        return result;
    }
};

#endif
