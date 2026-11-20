#pragma once

#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
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
                    using namespace MathVector;

                    [[nodiscard]] inline KernelTypes::Float Sin(KernelTypes::Float Value) noexcept
                    {
                        Vector4 Sine;
                        Vector4 Cosine;
                        Vector4::sReplicate(Value).SinCos(Sine, Cosine);
                        return Sine.GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float Cos(KernelTypes::Float Value) noexcept
                    {
                        Vector4 Sine;
                        Vector4 Cosine;
                        Vector4::sReplicate(Value).SinCos(Sine, Cosine);
                        return Cosine.GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float Tan(KernelTypes::Float Value) noexcept
                    {
                        return Vector4::sReplicate(Value).Tan().GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float ASin(KernelTypes::Float Value) noexcept
                    {
                        return Vector4::sReplicate(Value).ASin().GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float ACos(KernelTypes::Float Value) noexcept
                    {
                        return Vector4::sReplicate(Value).ACos().GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float ACosApproximate(KernelTypes::Float Value) noexcept
                    {
                        const KernelTypes::Float Absolute = Min(Abs(Value), 1.0f);
                        const KernelTypes::Float Result = Sqrt(1.0f - Absolute) * (HalfPi - 0.175394f * Absolute);
                        return Value < 0.0f ? Pi - Result : Result;
                    }

                    [[nodiscard]] inline KernelTypes::Float ATan(KernelTypes::Float Value) noexcept
                    {
                        return Vector4::sReplicate(Value).ATan().GetX();
                    }

                    [[nodiscard]] inline KernelTypes::Float ATan2(KernelTypes::Float Y, KernelTypes::Float X) noexcept
                    {
                        return Vector4::sATan2(Vector4::sReplicate(Y), Vector4::sReplicate(X)).GetX();
                    }
                }
            }
        }
    }
}
