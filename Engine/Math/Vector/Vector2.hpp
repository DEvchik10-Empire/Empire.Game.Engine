#pragma once

#include "../../../Kernel/Types.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Vector2
        {
            Types::f32 X;
            Types::f32 Y;

            constexpr Vector2() noexcept
                : X(0.0f), Y(0.0f)
            {
            }

            constexpr Vector2(Types::f32 X, Types::f32 Y) noexcept
                : X(X), Y(Y)
            {
            }

            [[nodiscard]]
            Types::f32 Length() const noexcept;

            [[nodiscard]]
            Types::f32 LengthSquared() const noexcept;

            [[nodiscard]]
            Vector2 Normalized() const noexcept;

            [[nodiscard]]
            Types::f32 Dot(const Vector2& Other) const noexcept;

            [[nodiscard]]
            Types::f32 Distance(const Vector2& Other) const noexcept;

            [[nodiscard]]
            Types::f32 DistanceSquared(const Vector2& Other) const noexcept;

            constexpr Vector2 operator+(const Vector2& Other) const noexcept
            {
                return { X + Other.X, Y + Other.Y };
            }

            constexpr Vector2 operator-(const Vector2& Other) const noexcept
            {
                return { X - Other.X, Y - Other.Y };
            }

            constexpr Vector2 operator*(Types::f32 Scalar) const noexcept
            {
                return { X * Scalar, Y * Scalar };
            }

            constexpr Vector2 operator/(Types::f32 Scalar) const noexcept
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

            constexpr Vector2& operator*=(Types::f32 Scalar) noexcept
            {
                X *= Scalar;
                Y *= Scalar;

                return *this;
            }

            constexpr Vector2& operator/=(Types::f32 Scalar) noexcept
            {
                X /= Scalar;
                Y /= Scalar;

                return *this;
            }

            constexpr Types::b8 operator==(const Vector2& Other) const noexcept
            {
                return X == Other.X && Y == Other.Y;
            }

            constexpr Types::b8 operator!=(const Vector2& Other) const noexcept
            {
                return !(*this == Other);
            }
        };

        constexpr Vector2 operator*(Types::f32 Scalar, const Vector2& Vector) noexcept
        {
            return Vector * Scalar;
        }
    }
}
