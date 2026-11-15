#pragma once

#include "Aliases.hpp"

using namespace Engine;

namespace Math
{
    struct Vector4
    {
        f32 X;
        f32 Y;
        f32 Z;
        f32 W;

        constexpr Vector4() noexcept
            : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
        {
        }

        constexpr Vector4(f32 X, f32 Y, f32 Z, f32 W) noexcept
            : X(X), Y(Y), Z(Z), W(W)
        {
        }

        [[nodiscard]]
        f32 Length() const noexcept;

        [[nodiscard]]
        f32 LengthSquared() const noexcept;

        [[nodiscard]]
        Vector4 Normalized() const noexcept;

        [[nodiscard]]
        f32 Dot(const Vector4& Other) const noexcept;

        [[nodiscard]]
        f32 Distance(const Vector4& Other) const noexcept;

        [[nodiscard]]
        f32 DistanceSquared(const Vector4& Other) const noexcept;

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

        constexpr Vector4 operator*(f32 Scalar) const noexcept
        {
            return {
                X * Scalar,
                Y * Scalar,
                Z * Scalar,
                W * Scalar
            };
        }

        constexpr Vector4 operator/(f32 Scalar) const noexcept
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

        constexpr Vector4& operator*=(f32 Scalar) noexcept
        {
            X *= Scalar;
            Y *= Scalar;
            Z *= Scalar;
            W *= Scalar;

            return *this;
        }

        constexpr Vector4& operator/=(f32 Scalar) noexcept
        {
            X /= Scalar;
            Y /= Scalar;
            Z /= Scalar;
            W /= Scalar;

            return *this;
        }

        constexpr b8 operator==(const Vector4& Other) const noexcept
        {
            return X == Other.X &&
                   Y == Other.Y &&
                   Z == Other.Z &&
                   W == Other.W;
        }

        constexpr b8 operator!=(const Vector4& Other) const noexcept
        {
            return !(*this == Other);
        }
    };

    constexpr Vector4 operator*(f32 Scalar, const Vector4& Vector) noexcept
    {
        return Vector * Scalar;
    }
}