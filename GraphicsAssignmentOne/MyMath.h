#pragma once
#include "Defines.h"
#include <cmath>

constexpr float DEG2RAD = 3.14159265f / 180.0f;

// === Matrix Constructors ===
Matrix4x4 CreateIdentityMatrix()
{
    Matrix4x4 m = {};
    for (int i = 0; i < 4; ++i)
        m.m[i][i] = 1.0f;
    return m;
}

Matrix4x4 CreateTranslationMatrix(float x, float y, float z)
{
    Matrix4x4 m = CreateIdentityMatrix();
    m.m[3][0] = x;
    m.m[3][1] = y;
    m.m[3][2] = z;
    return m;
}

Matrix4x4 CreateRotationXMatrix(float angleRadians) 
{
    Matrix4x4 m = CreateIdentityMatrix();
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);
    m.m[1][1] = c;
    m.m[1][2] = s;
    m.m[2][1] = -s;
    m.m[2][2] = c;
    return m;
}

Matrix4x4 CreateRotationYMatrix(float angleRadians) 
{
    Matrix4x4 m = CreateIdentityMatrix();
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);
    m.m[0][0] = c;
    m.m[0][2] = -s;
    m.m[2][0] = s;
    m.m[2][2] = c;
    return m;
}

Matrix4x4 CreatePerspectiveMatrix(float fovYRadians, float aspect, float nearZ, float farZ) 
{
    Matrix4x4 m = {};
    float f = 1.0f / std::tan(fovYRadians * 0.5f);
    m.m[0][0] = f / aspect;
    m.m[1][1] = f;
    m.m[2][2] = farZ / (farZ - nearZ);
    m.m[2][3] = 1.0f;
    m.m[3][2] = (-nearZ * farZ) / (farZ - nearZ);
    return m;
}

// Inverts a matrix that only has rotation + translation
Matrix4x4 InverseRigidBodyMatrix(const Matrix4x4& m) 
{
    Matrix4x4 inv = CreateIdentityMatrix();
    // Transpose the 3x3 rotation part
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            inv.m[r][c] = m.m[c][r];

    // Inverse translation
    inv.m[3][0] = -(m.m[3][0] * inv.m[0][0] + m.m[3][1] * inv.m[1][0] + m.m[3][2] * inv.m[2][0]);
    inv.m[3][1] = -(m.m[3][0] * inv.m[0][1] + m.m[3][1] * inv.m[1][1] + m.m[3][2] * inv.m[2][1]);
    inv.m[3][2] = -(m.m[3][0] * inv.m[0][2] + m.m[3][1] * inv.m[1][2] + m.m[3][2] * inv.m[2][2]);

    return inv;
}

// Matrix multiplication
void MatrixMatrixMultiply(Matrix4x4& out, const Matrix4x4& a, const Matrix4x4& b)
{
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            out.m[r][c] = 0.0f;
            for (int k = 0; k < 4; ++k)
                out.m[r][c] += a.m[r][k] * b.m[k][c];
        }
    }
}

// Vector transform
Float4 MatrixVertexMultiply(const Float4& v, const Matrix4x4& m)
{
    Float4 out;
    out.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
    out.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
    out.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
    out.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
    return out;
}

ScreenXY NDCtoScreen(const Float4& ndc)
{
    ScreenXY screen;
    screen.x = static_cast<int>((ndc.x * 0.5f + 0.5f) * SCREEN_WIDTH);
    screen.y = static_cast<int>((-ndc.y * 0.5f + 0.5f) * SCREEN_HEIGHT);
    return screen;
}
