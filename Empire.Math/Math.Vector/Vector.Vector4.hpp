#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathVector
            {
                using namespace EmpireKernel;

                struct Vector4
                {
                    KernelTypes::Float X;
                    KernelTypes::Float Y;
                    KernelTypes::Float Z;
                    KernelTypes::Float W;

                    constexpr Vector4() noexcept
                        : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
                    {
                    }

                    constexpr Vector4(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z, KernelTypes::Float W) noexcept
                        : X(X), Y(Y), Z(Z), W(W)
                    {
                    }

                    [[nodiscard]]
                    KernelTypes::Float Length() const noexcept;

                    [[nodiscard]]
                    KernelTypes::Float LengthSquared() const noexcept;

                    [[nodiscard]]
                    Vector4 Normalized() const noexcept;

                    [[nodiscard]]
                    KernelTypes::Float Dot(const Vector4& Other) const noexcept;

                    [[nodiscard]]
                    KernelTypes::Float Distance(const Vector4& Other) const noexcept;

                    [[nodiscard]]
                    KernelTypes::Float DistanceSquared(const Vector4& Other) const noexcept;

                    constexpr Vector4 operator+(const Vector4& Other) const noexcept
                    {
                        return {
                            X + Other.X,
                            Y + Other.Y,
                            Z + Other.Z,
                            W + Other.W
                        };
                    }

                    constexpr Vector4 operator-(const Vector4& Other) const noexcept
                    {
                        return {
                            X - Other.X,
                            Y - Other.Y,
                            Z - Other.Z,
                            W - Other.W
                        };
                    }

                    constexpr Vector4 operator*(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            X * Scalar,
                            Y * Scalar,
                            Z * Scalar,
                            W * Scalar
                        };
                    }

                    constexpr Vector4 operator/(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            X / Scalar,
                            Y / Scalar,
                            Z / Scalar,
                            W / Scalar
                        };
                    }

                    constexpr Vector4 operator-() const noexcept
                    {
                        return {
                            -X,
                            -Y,
                            -Z,
                            -W
                        };
                    }

                    constexpr Vector4& operator+=(const Vector4& Other) noexcept
                    {
                        X += Other.X;
                        Y += Other.Y;
                        Z += Other.Z;
                        W += Other.W;

                        return *this;
                    }

                    constexpr Vector4& operator-=(const Vector4& Other) noexcept
                    {
                        X -= Other.X;
                        Y -= Other.Y;
                        Z -= Other.Z;
                        W -= Other.W;

                        return *this;
                    }

                    constexpr Vector4& operator*=(KernelTypes::Float Scalar) noexcept
                    {
                        X *= Scalar;
                        Y *= Scalar;
                        Z *= Scalar;
                        W *= Scalar;

                        return *this;
                    }

                    constexpr Vector4& operator/=(KernelTypes::Float Scalar) noexcept
                    {
                        X /= Scalar;
                        Y /= Scalar;
                        Z /= Scalar;
                        W /= Scalar;

                        return *this;
                    }

                    constexpr KernelTypes::Bool operator==(const Vector4& Other) const noexcept
                    {
                        return X == Other.X &&
                               Y == Other.Y &&
                               Z == Other.Z &&
                               W == Other.W;
                    }

                    constexpr KernelTypes::Bool operator!=(const Vector4& Other) const noexcept
                    {
                        return !(*this == Other);
                    }
                };

                constexpr Vector4 operator*(KernelTypes::Float Scalar, const Vector4& Vector) noexcept
                {
                    return Vector * Scalar;
                }
            }
        }
    }
}
