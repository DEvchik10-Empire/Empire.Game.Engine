#include "Transform.hpp"

namespace Math
{
    Transform::Transform()
        : Position(0.0f, 0.0f, 0.0f)
        , Rotation(Quaternion::Identity())
        , Scale(1.0f, 1.0f, 1.0f)
    {
    }

    Transform::Transform(
        const Vector3& Position,
        const Quaternion& Rotation,
        const Vector3& Scale
    )
        : Position(Position)
        , Rotation(Rotation)
        , Scale(Scale)
    {
    }

    Transform Transform::Identity()
    {
        return {
            Vector3(0.0f, 0.0f, 0.0f),
            Quaternion::Identity(),
            Vector3(1.0f, 1.0f, 1.0f)
        };
    }

    Matrix4 Transform::ToMatrix4() const
    {
        const Matrix4 TranslationMatrix =
            Matrix4::Translation(Position);

        const Matrix4 RotationMatrix =
            Rotation.ToMatrix4();

        const Matrix4 ScaleMatrix =
            Matrix4::Scale(Scale);

        return TranslationMatrix *
               RotationMatrix *
               ScaleMatrix;
    }

    Vector3 Transform::TransformPoint(
        const Vector3& Point
    ) const
    {
        const Vector4 Result =
            ToMatrix4() *
            Vector4(
                Point.X,
                Point.Y,
                Point.Z,
                1.0f
            );

        return {
            Result.X,
            Result.Y,
            Result.Z
        };
    }

    Vector3 Transform::TransformDirection(
        const Vector3& Direction
    ) const
    {
        const Vector4 Result =
            ToMatrix4() *
            Vector4(
                Direction.X,
                Direction.Y,
                Direction.Z,
                0.0f
            );

        return {
            Result.X,
            Result.Y,
            Result.Z
        };
    }

    Transform Transform::Inversed() const
    {
        Transform Result;

        Result.Scale = {
            1.0f / Scale.X,
            1.0f / Scale.Y,
            1.0f / Scale.Z
        };

        Result.Rotation = Rotation.Inversed();

        Result.Position = Result.Rotation.Rotate(
            {
                -Position.X * Result.Scale.X,
                -Position.Y * Result.Scale.Y,
                -Position.Z * Result.Scale.Z
            }
        );

        return Result;
    }

    Transform Transform::operator*(
        const Transform& Other
    ) const
    {
        Transform Result;

        Result.Scale = {
            Scale.X * Other.Scale.X,
            Scale.Y * Other.Scale.Y,
            Scale.Z * Other.Scale.Z
        };

        Result.Rotation =
            Rotation *
            Other.Rotation;

        Result.Position =
            Position +
            Rotation.Rotate(
                {
                    Other.Position.X * Scale.X,
                    Other.Position.Y * Scale.Y,
                    Other.Position.Z * Scale.Z
                }
            );

        return Result;
    }

    Transform& Transform::operator*=(
        const Transform& Other
    )
    {
        *this = *this * Other;

        return *this;
    }

    b8 Transform::operator==(
        const Transform& Other
    ) const
    {
        return
            Position == Other.Position &&
            Rotation == Other.Rotation &&
            Scale == Other.Scale;
    }

    b8 Transform::operator!=(
        const Transform& Other
    ) const
    {
        return !(*this == Other);
    }
}