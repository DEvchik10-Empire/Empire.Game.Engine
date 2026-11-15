#include "Empire.Math/Math.Geometry/Geometry.3D/3D.CatmullRom.hpp"

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

                    Vector3 CatmullRom::Interpolate(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, KernelTypes::Float T) noexcept
                    {
                        const KernelTypes::Float T2 = T * T;
                        const KernelTypes::Float T3 = T2 * T;

                        return (P1 * 2.0f +
                                (P2 - P0) * T +
                                (P0 * 2.0f - P1 * 5.0f + P2 * 4.0f - P3) * T2 +
                                (P1 * 3.0f - P0 - P2 * 3.0f + P3) * T3) * 0.5f;
                    }
                }
            }
        }
    }
}
