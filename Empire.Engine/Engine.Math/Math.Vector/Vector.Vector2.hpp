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

                    struct Vector2
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;

                        constexpr Vector2() noexcept
                            : X(0.0f), Y(0.0f)
                        {
                        }

                        constexpr Vector2(KernelTypes::Float X, KernelTypes::Float Y) noexcept
                            : X(X), Y(Y)
                        {
                        }

                        [[nodiscard]]
                        KernelTypes::Float Length() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float LengthSquared() const noexcept;

                        [[nodiscard]]
                        Vector2 Normalized() const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float Dot(const Vector2& Other) const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float Distance(const Vector2& Other) const noexcept;

                        [[nodiscard]]
                        KernelTypes::Float DistanceSquared(const Vector2& Other) const noexcept;

                        constexpr Vector2 operator+(const Vector2& Other) const noexcept
                        {
                            return { X + Other.X, Y + Other.Y };
                        }

                        constexpr Vector2 operator-(const Vector2& Other) const noexcept
                        {
                            return { X - Other.X, Y - Other.Y };
                        }

                        constexpr Vector2 operator*(KernelTypes::Float Scalar) const noexcept
                        {
                            return { X * Scalar, Y * Scalar };
                        }

                        constexpr Vector2 operator/(KernelTypes::Float Scalar) const noexcept
                        {
                            return { X / Scalar, Y / Scalar };
                        }

                        constexpr Vector2 operator-() const noexcept
                        {
                            return { -X, -Y };
                        }

                        constexpr Vector2& operator+=(const Vector2& Other) noexcept
                        {
                            X += Other.X;
                            Y += Other.Y;

                            return *this;
                        }

                        constexpr Vector2& operator-=(const Vector2& Other) noexcept
                        {
                            X -= Other.X;
                            Y -= Other.Y;

                            return *this;
                        }

                        constexpr Vector2& operator*=(KernelTypes::Float Scalar) noexcept
                        {
                            X *= Scalar;
                            Y *= Scalar;

                            return *this;
                        }

                        constexpr Vector2& operator/=(KernelTypes::Float Scalar) noexcept
                        {
                            X /= Scalar;
                            Y /= Scalar;

                            return *this;
                        }

                        constexpr KernelTypes::Bool operator==(const Vector2& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y;
                        }

                        constexpr KernelTypes::Bool operator!=(const Vector2& Other) const noexcept
                        {
                            return !(*this == Other);
                        }
                    };

                    constexpr Vector2 operator*(KernelTypes::Float Scalar, const Vector2& Vector) noexcept
                    {
                        return Vector * Scalar;
                    }
                }
            }
        }
    }
}
