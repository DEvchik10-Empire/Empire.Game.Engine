#pragma once

/*
 * Минимальная математика для теста сцены.
 * Матрицы column-major (как в GLSL/Vulkan): m[столбец * 4 + строка].
 * Это именно то, что ждут шейдеры в Shaders/ (4 столбца float4).
 *
 * Это временная замена: когда Engine/Math будет готов для рендера,
 * сцена перейдёт на него.
 */

#include <cmath>

namespace SceneMath
{
    struct Vec3
    {
        float x = 0, y = 0, z = 0;
    };

    inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
    inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
    inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }

    inline float Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

    inline Vec3 Cross(Vec3 a, Vec3 b)
    {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    inline Vec3 Normalize(Vec3 v)
    {
        const float length = std::sqrt(Dot(v, v));
        return length > 1e-12f ? v * (1.0f / length) : Vec3{0, 1, 0};
    }

    struct Mat4
    {
        float m[16] = {};

        static Mat4 Identity()
        {
            Mat4 r;
            r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
            return r;
        }
    };

    // a * b (сначала применяется b).
    inline Mat4 Multiply(const Mat4& a, const Mat4& b)
    {
        Mat4 r;

        for (int c = 0; c < 4; ++c)
        {
            for (int row = 0; row < 4; ++row)
            {
                float sum = 0.0f;

                for (int k = 0; k < 4; ++k)
                {
                    sum += a.m[k * 4 + row] * b.m[c * 4 + k];
                }

                r.m[c * 4 + row] = sum;
            }
        }

        return r;
    }

    // Vulkan: глубина 0..1, ось Y экрана направлена вниз (поэтому -f).
    inline Mat4 Perspective(float fovY, float aspect, float nearPlane, float farPlane)
    {
        const float f = 1.0f / std::tan(fovY * 0.5f);

        Mat4 r;
        r.m[0] = f / aspect;
        r.m[5] = -f;
        r.m[10] = farPlane / (nearPlane - farPlane);
        r.m[11] = -1.0f;
        r.m[14] = (nearPlane * farPlane) / (nearPlane - farPlane);
        return r;
    }

    // Обратная к Perspective (выведена аналитически, см. SceneMain.cpp).
    inline Mat4 InversePerspective(float fovY, float aspect, float nearPlane, float farPlane)
    {
        const float f = 1.0f / std::tan(fovY * 0.5f);
        const float a = farPlane / (nearPlane - farPlane);
        const float b = (nearPlane * farPlane) / (nearPlane - farPlane);

        Mat4 r;
        r.m[0] = -aspect / f;
        r.m[5] = 1.0f / f;
        r.m[11] = -1.0f / b;
        r.m[14] = 1.0f;
        r.m[15] = -a / b;
        return r;
    }

    inline Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up)
    {
        const Vec3 f = Normalize(center - eye);
        const Vec3 s = Normalize(Cross(f, up));
        const Vec3 u = Cross(s, f);

        Mat4 r = Mat4::Identity();
        r.m[0] = s.x;  r.m[4] = s.y;  r.m[8]  = s.z;  r.m[12] = -Dot(s, eye);
        r.m[1] = u.x;  r.m[5] = u.y;  r.m[9]  = u.z;  r.m[13] = -Dot(u, eye);
        r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z; r.m[14] = Dot(f, eye);
        return r;
    }

    // Обратная к LookAt: камера -> мир.
    inline Mat4 InverseLookAt(Vec3 eye, Vec3 center, Vec3 up)
    {
        const Vec3 f = Normalize(center - eye);
        const Vec3 s = Normalize(Cross(f, up));
        const Vec3 u = Cross(s, f);

        Mat4 r = Mat4::Identity();
        r.m[0] = s.x;  r.m[1] = s.y;  r.m[2] = s.z;
        r.m[4] = u.x;  r.m[5] = u.y;  r.m[6] = u.z;
        r.m[8] = -f.x; r.m[9] = -f.y; r.m[10] = -f.z;
        r.m[12] = eye.x; r.m[13] = eye.y; r.m[14] = eye.z;
        return r;
    }

    // Поворот вокруг оси Y.
    inline Mat4 RotationY(float angle)
    {
        const float c = std::cos(angle);
        const float s = std::sin(angle);

        Mat4 r = Mat4::Identity();
        r.m[0] = c;  r.m[8] = s;
        r.m[2] = -s; r.m[10] = c;
        return r;
    }

    inline Mat4 RotationX(float angle)
    {
        const float c = std::cos(angle);
        const float s = std::sin(angle);

        Mat4 r = Mat4::Identity();
        r.m[5] = c;  r.m[9] = -s;
        r.m[6] = s;  r.m[10] = c;
        return r;
    }

    inline Mat4 Scale(float s)
    {
        Mat4 r = Mat4::Identity();
        r.m[0] = r.m[5] = r.m[10] = s;
        return r;
    }
}
