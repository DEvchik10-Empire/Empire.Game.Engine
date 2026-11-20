#pragma once

// Quadratic root solver ported from Jolt Physics 5.7.0 (Math/FindRoot.h).
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

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
                    template <typename T>
                    [[nodiscard]] inline KernelTypes::Int32 FindRoot(
                        T A,
                        T B,
                        T C,
                        T& Root1,
                        T& Root2
                    ) noexcept
                    {
                        if (A == static_cast<T>(0))
                        {
                            if (B == static_cast<T>(0))
                            {
                                if (C == static_cast<T>(0))
                                {
                                    Root1 = Root2 = static_cast<T>(0);
                                    return 1;
                                }
                                return 0;
                            }

                            Root1 = Root2 = -C / B;
                            return 1;
                        }

                        const T Discriminant = DifferenceOfProducts(B, B, static_cast<T>(4) * A, C);
                        if (Discriminant < static_cast<T>(0))
                            return 0;

                        const T Q = (B + Sign(B) * Sqrt(Discriminant)) / static_cast<T>(-2);
                        Root1 = Q / A;
                        if (Q == static_cast<T>(0))
                        {
                            Root2 = Root1;
                            return 1;
                        }

                        Root2 = C / Q;
                        return 2;
                    }
                }
            }
        }
    }
}
