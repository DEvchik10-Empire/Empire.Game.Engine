#include "CatmullRom.hpp"

namespace Math
{
    Vector3 CatmullRom::Interpolate(
        const Vector3& P0,
        const Vector3& P1,
        const Vector3& P2,
        const Vector3& P3,
        f32 T
    ) noexcept
    {
        const f32 T2 = T * T;
        const f32 T3 = T2 * T;

        return (P1 * 2.0f +
                (P2 - P0) * T +
                (P0 * 2.0f - P1 * 5.0f + P2 * 4.0f - P3) * T2 +
                (P1 * 3.0f - P0 - P2 * 3.0f + P3) * T3) * 0.5f;
    }
}