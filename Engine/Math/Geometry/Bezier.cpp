#include "Bezier.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Vector3 Bezier::Cubic(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, Types::f32 T) noexcept
        {
            const Types::f32 OneMinusT = 1.0f - T;

            const Types::f32 A = OneMinusT * OneMinusT * OneMinusT;
            const Types::f32 B = 3.0f * OneMinusT * OneMinusT * T;
            const Types::f32 C = 3.0f * OneMinusT * T * T;
            const Types::f32 D = T * T * T;

            return P0 * A + P1 * B + P2 * C + P3 * D;
        }
    }
}
