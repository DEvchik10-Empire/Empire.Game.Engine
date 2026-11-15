#include "Vector2.hpp"

#include <cmath>

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Types::f32 Vector2::Length() const noexcept
        {
            return std::sqrt(LengthSquared());
        }

        Types::f32 Vector2::LengthSquared() const noexcept
        {
            return X * X + Y * Y;
        }

        Vector2 Vector2::Normalized() const noexcept
        {
            const Types::f32 Length = this->Length();

            if (Length == 0.0f)
                return {};

            return *this / Length;
        }

        Types::f32 Vector2::Dot(const Vector2& Other) const noexcept
        {
            return X * Other.X + Y * Other.Y;
        }

        Types::f32 Vector2::Distance(const Vector2& Other) const noexcept
        {
            return (*this - Other).Length();
        }

        Types::f32 Vector2::DistanceSquared(const Vector2& Other) const noexcept
        {
            return (*this - Other).LengthSquared();
        }
    }
}
