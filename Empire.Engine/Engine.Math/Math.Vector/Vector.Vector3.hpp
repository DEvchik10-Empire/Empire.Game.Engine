#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

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
                    using namespace EngineKernel;

                    struct Vector3
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;
                        KernelTypes::Float Z;

                        constexpr Vector3() noexcept
                            : X(0.0f), Y(0.0f), Z(0.0f)
                        {
                        }

                        constexpr Vector3(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z) noexcept
                            : X(X), Y(Y), Z(Z)
                        {
                        }

                        [[nodiscard]]
                        KernelTypes::Float Length() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float LengthSquared() const noexcept;

                        [[nodiscard]]
                        Vector3 Normalized() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float Dot(const Vector3& Other) const noexcept;

                        [[nodiscard]]
                        Vector3 Cross(const Vector3& Other) const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float Distance(const Vector3& Other) const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float DistanceSquared(const Vector3& Other) const noexcept;

                        constexpr Vector3 operator+(const Vector3& Other) const noexcept
                        {
                            return {
                                X + Other.X,
                                Y + Other.Y,
                                Z + Other.Z
                            };
                        }

                        constexpr Vector3 operator-(const Vector3& Other) const noexcept
                        {
                            return {
                                X - Other.X,
                                Y - Other.Y,
                                Z - Other.Z
                            };
                        }

                        constexpr Vector3 operator*(KernelTypes::Float Scalar) const noexcept
                        {
                            return {
                                X * Scalar,
                                Y * Scalar,
                                Z * Scalar
                            };
                        }

                        constexpr Vector3 operator/(KernelTypes::Float Scalar) const noexcept
                        {
                            return {
                                X / Scalar,
                                Y / Scalar,
                                Z / Scalar
                            };
                        }

                        constexpr Vector3 operator-() const noexcept
                        {
                            return {
                                -X,
                                -Y,
                                -Z
                            };
                        }

                        constexpr Vector3& operator+=(const Vector3& Other) noexcept
                        {
                            X += Other.X;
                            Y += Other.Y;
                            Z += Other.Z;

                            return *this;
                        }

                        constexpr Vector3& operator-=(const Vector3& Other) noexcept
                        {
                            X -= Other.X;
                            Y -= Other.Y;
                            Z -= Other.Z;

                            return *this;
                        }

                        constexpr Vector3& operator*=(KernelTypes::Float Scalar) noexcept
                        {
                            X *= Scalar;
                            Y *= Scalar;
                            Z *= Scalar;

                            return *this;
                        }

                        constexpr Vector3& operator/=(KernelTypes::Float Scalar) noexcept
                        {
                            X /= Scalar;
                            Y /= Scalar;
                            Z /= Scalar;

                            return *this;
                        }

                        constexpr KernelTypes::Bool operator==(const Vector3& Other) const noexcept
                        {
                            return X == Other.X &&
                                   Y == Other.Y &&
                                   Z == Other.Z;
                        }

                        constexpr KernelTypes::Bool operator!=(const Vector3& Other) const noexcept
                        {
                            return !(*this == Other);
                        }
                    };

                    constexpr Vector3 operator*(KernelTypes::Float Scalar, const Vector3& Vector) noexcept
                    {
                        return Vector * Scalar;
                    }
                }
            }
        }
    }
}
