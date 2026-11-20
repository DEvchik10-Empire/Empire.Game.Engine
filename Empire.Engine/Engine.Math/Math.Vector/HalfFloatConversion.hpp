#pragma once

// Packed four-lane HalfFloat conversion from Jolt Physics 5.7.0 (Math/HalfFloat.h).
#include "Empire.Engine/Engine.Math/Math.Scalar/HalfFloat.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathScalar
                {
                    namespace HalfFloatConversion
                    {
                        [[nodiscard]] inline MathVector::Vector4 ToFloat(const MathVector::UVector4& Packed) noexcept
                        {
                            const MathVector::UVector4 Values = Packed.Expand4Uint16Lo();
                            return MathVector::Vector4(
                                ToFloat(static_cast<HalfFloat>(Values[0])),
                                ToFloat(static_cast<HalfFloat>(Values[1])),
                                ToFloat(static_cast<HalfFloat>(Values[2])),
                                ToFloat(static_cast<HalfFloat>(Values[3]))
                            );
                        }
                    }
                }
            }
        }
    }
}
