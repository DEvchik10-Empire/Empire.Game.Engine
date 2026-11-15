#pragma once

#include "../../../Kernel/Types.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Vector3
        {
            Types::f32 X;
            Types::f32 Y;
            Types::f32 Z;

            constexpr Vector3() noexcept
                : X(0.0f), Y(0.0f), Z(0.0f)
            {
            }

            constexpr Vector3(Types::f32 X, Types::f32 Y, Types::f32 Z) noexcept
                : X(X), Y(Y), Z(Z)
            {
            }

            [[nodiscard]]
            Types::f32 Length() const noexcept;

            [[nodiscard]]
            Types::f32 LengthSquared() const noexcept;

            [[nodiscard]]
            Vector3 Normalized() const noexcept;

            [[nodiscard]]
            Types::f32 Dot(const Vector3& Other) const noexcept;

            [[nodiscard]]
            Vector3 Cross(const Vector3& Other) const noexcept;

            [[nodiscard]]
            Types::f32 Distance(const Vector3& Other) const noexcept;

            [[nodiscard]]
            Types::f32 DistanceSquared(const Vector3& Other) const noexcept;

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

            constexpr Vector3 operator*(Types::f32 Scalar) const noexcept
            {
                return {
                    X * Scalar,
                    Y * Scalar,
                    Z * Scalar
                };
            }

            constexpr Vector3 operator/(Types::f32 Scalar) const noexcept
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

            constexpr Vector3& operator*=(Types::f32 Scalar) noexcept
            {
                X *= Scalar;
                Y *= Scalar;
                Z *= Scalar;

                return *this;
            }

            constexpr Vector3& operator/=(Types::f32 Scalar) noexcept
            {
                X /= Scalar;
                Y /= Scalar;
                Z /= Scalar;

                return *this;
            }

            constexpr Types::b8 operator==(const Vector3& Other) const noexcept
            {
                return X == Other.X &&
                       Y == Other.Y &&
                       Z == Other.Z;
            }

            constexpr Types::b8 operator!=(const Vector3& Other) const noexcept
            {
                return !(*this == Other);
            }
        };

        constexpr Vector3 operator*(Types::f32 Scalar, const Vector3& Vector) noexcept
        {
            return Vector * Scalar;
        }
    }
}
