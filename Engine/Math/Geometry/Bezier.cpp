#include "Bezier.hpp"

namespace Math
{
    Vector3 Bezier::Cubic(
        const Vector3& P0,
        const Vector3& P1,
        const Vector3& P2,
        const Vector3& P3,
        f32 T
    ) noexcept
    {
        const f32 OneMinusT = 1.0f - T;

        const f32 A = OneMinusT * OneMinusT * OneMinusT;
        const f32 B = 3.0f * OneMinusT * OneMinusT * T;
        const f32 C = 3.0f * OneMinusT * T * T;
        const f32 D = T * T * T;

        return P0 * A + P1 * B + P2 * C + P3 * D;
    }
}