#pragma once

#include <cmath>
#include <initializer_list>
#include <cstdio>

// Общие проверки для тестов Math. Счётчики одни на весь исполняемый файл (inline-переменные).
namespace MathTests
{
    inline int Checks = 0;
    inline int Failures = 0;

    inline bool Near(float A, float B, float Tolerance = 1e-4f)
    {
        return std::fabs(A - B) <= Tolerance;
    }

    // Детерминированный генератор, чтобы тесты воспроизводились.
    struct Random
    {
        unsigned State;

        explicit Random(unsigned Seed) : State(Seed ? Seed : 1u) {}

        unsigned Next()
        {
            State ^= State << 13;
            State ^= State >> 17;
            State ^= State << 5;
            return State;
        }

        float Range(float Minimum, float Maximum)
        {
            return Minimum + (Maximum - Minimum) * (static_cast<float>(Next() & 0xFFFFFF) / 16777215.0f);
        }
    };
}

#define SECTION(Name) std::printf("[%s]\n", Name)

#define CHECK(Condition) \
    do { \
        ++MathTests::Checks; \
        if (!(Condition)) \
        { \
            ++MathTests::Failures; \
            std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #Condition); \
        } \
    } while (0)

#define CHECK_NEAR(A, B) CHECK(MathTests::Near((A), (B)))
#define CHECK_NEAR_TOL(A, B, Tolerance) CHECK(MathTests::Near((A), (B), (Tolerance)))

#define CHECK_VEC2(V, Px, Py) CHECK(MathTests::Near((V).X, (Px)) && MathTests::Near((V).Y, (Py)))
#define CHECK_VEC3(V, Px, Py, Pz) CHECK(MathTests::Near((V).GetX(), (Px)) && MathTests::Near((V).GetY(), (Py)) && MathTests::Near((V).GetZ(), (Pz)))
#define CHECK_VEC4(V, Px, Py, Pz, Pw) CHECK(MathTests::Near((V).X, (Px)) && MathTests::Near((V).Y, (Py)) && MathTests::Near((V).Z, (Pz)) && MathTests::Near((V).W, (Pw)))
