#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Config/Config.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathVector
                {
                    struct Float3;
                    struct Double3;
                    class Vector3;
                    class Vector4;
                    class UVector4;
                    class BVector16;
                    class DVector3;
                }

                namespace MathMatrix
                {
                    class Matrix4;
                    class DMatrix4;
                }

                namespace MathQuaternion
                {
                    class Quaternion;
                }

                namespace MathScalar
                {
                    using namespace EngineKernel;
                    using Real = KernelConfig::Real;

                    template <KernelTypes::Bool UseDouble>
                    struct RealTypes;

                    template <>
                    struct RealTypes<false>
                    {
                        using Storage3 = MathVector::Float3;
                        using Vector3 = MathVector::Vector3;
                        using Matrix4 = MathMatrix::Matrix4;
                    };

                    template <>
                    struct RealTypes<true>
                    {
                        using Storage3 = MathVector::Double3;
                        using Vector3 = MathVector::DVector3;
                        using Matrix4 = MathMatrix::DMatrix4;
                    };

                    using ConfiguredRealTypes = RealTypes<KernelConfig::WorldUsesDoublePrecision>;
                    using Real3 = typename ConfiguredRealTypes::Storage3;
                    using RVector3 = typename ConfiguredRealTypes::Vector3;
                    using RVector3Arg = const RVector3&;
                    using RMatrix4 = typename ConfiguredRealTypes::Matrix4;
                    using RMatrix4Arg = const RMatrix4&;

                    using Vector3Arg = const MathVector::Vector3&;
                    using Vector4Arg = const MathVector::Vector4&;
                    using UVector4Arg = const MathVector::UVector4&;
                    using BVector16Arg = const MathVector::BVector16&;
                    using DVector3Arg = const MathVector::DVector3&;
                    using QuaternionArg = const MathQuaternion::Quaternion&;
                    using Matrix4Arg = const MathMatrix::Matrix4&;
                    using DMatrix4Arg = const MathMatrix::DMatrix4&;
                }
            }
        }
    }
}
