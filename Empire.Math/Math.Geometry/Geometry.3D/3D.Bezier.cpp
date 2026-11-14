#include "Empire.Math/Math.Geometry/Geometry.3D/3D.Bezier.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathGeometry
            {
                namespace Geometry3D
                {
                    using namespace EmpireKernel;

                    Vector3 Bezier::Cubic(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, KernelTypes::Float T) noexcept
                    {
                        const KernelTypes::Float OneMinusT = 1.0f - T;

                        const KernelTypes::Float A = OneMinusT * OneMinusT * OneMinusT;
                        const KernelTypes::Float B = 3.0f * OneMinusT * OneMinusT * T;
                        const KernelTypes::Float C = 3.0f * OneMinusT * T * T;
                        const KernelTypes::Float D = T * T * T;

                        return P0 * A + P1 * B + P2 * C + P3 * D;
                    }
                }
            }
        }
    }
}
