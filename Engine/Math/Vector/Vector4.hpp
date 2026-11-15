#pragma once

#include "../../../Kernel/Types.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Vector4
        {
            Types::f32 X;
            Types::f32 Y;
            Types::f32 Z;
            Types::f32 W;

            constexpr Vector4() noexcept
                : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
            {
            }

            constexpr Vector4(Types::f32 X, Types::f32 Y, Types::f32 Z, Types::f32 W) noexcept
                : X(X), Y(Y), Z(Z), W(W)
            {
            }

            [[nodiscard]]
            Types::f32 Length() const noexcept;

            [[nodiscard]]
            Types::f32 LengthSquared() const noexcept;

            [[nodiscard]]
            Vector4 Normalized() const noexcept;

            [[nodiscard]]
            Types::f32 Dot(const Vector4& Other) const noexcept;

            [[nodiscard]]
            Types::f32 Distance(const Vector4& Other) const noexcept;

            [[nodiscard]]
            Types::f32 DistanceSquared(const Vector4& Other) const noexcept;

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

            constexpr Vector4 operator*(Types::f32 Scalar) const noexcept
            {
                return {
                    X * Scalar,
                    Y * Scalar,
                    Z * Scalar,
                    W * Scalar
                };
            }

            constexpr Vector4 operator/(Types::f32 Scalar) const noexcept
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

            constexpr Vector4& operator*=(Types::f32 Scalar) noexcept
            {
                X *= Scalar;
                Y *= Scalar;
                Z *= Scalar;
                W *= Scalar;

                return *this;
            }

            constexpr Vector4& operator/=(Types::f32 Scalar) noexcept
            {
                X /= Scalar;
                Y /= Scalar;
                Z /= Scalar;
                W /= Scalar;

                return *this;
            }

            constexpr Types::b8 operator==(const Vector4& Other) const noexcept
            {
                return X == Other.X &&
                       Y == Other.Y &&
                       Z == Other.Z &&
                       W == Other.W;
            }

            constexpr Types::b8 operator!=(const Vector4& Other) const noexcept
            {
                return !(*this == Other);
            }
        };

        constexpr Vector4 operator*(Types::f32 Scalar, const Vector4& Vector) noexcept
        {
            return Vector * Scalar;
        }
    }
}
