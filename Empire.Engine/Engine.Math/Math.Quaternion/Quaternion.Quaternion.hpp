#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathQuaternion
                {
                    using namespace EngineKernel;
                    using MathMatrix::Matrix4;
                    using MathVector::Vector3;

                    struct Quaternion
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;
                        KernelTypes::Float Z;
                        KernelTypes::Float W;

                        Quaternion();
                        Quaternion(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z, KernelTypes::Float W);

                        static Quaternion Identity();

                        static Quaternion FromAxisAngle(const Vector3& Axis, KernelTypes::Float Angle);

                        static Quaternion FromEulerAngles(const Vector3& EulerAngles);

                        KernelTypes::Float Length() const;
                        KernelTypes::Float LengthSquared() const;

                        Quaternion Normalized() const;
                        void Normalize();

                        Quaternion Conjugated() const;
                        Quaternion Inversed() const;

                        KernelTypes::Float Dot(const Quaternion& Other) const;

                        Vector3 Rotate(const Vector3& Vector) const;

                        Matrix4 ToMatrix4() const;

                        Vector3 ToEulerAngles() const;

                        Quaternion operator+(const Quaternion& Other) const;
                        Quaternion operator-(const Quaternion& Other) const;

                        Quaternion operator*(const Quaternion& Other) const;
                        Quaternion operator*(KernelTypes::Float Scalar) const;
                        Quaternion operator/(KernelTypes::Float Scalar) const;

                        Quaternion& operator+=(const Quaternion& Other);
                        Quaternion& operator-=(const Quaternion& Other);

                        Quaternion& operator*=(const Quaternion& Other);
                        Quaternion& operator*=(KernelTypes::Float Scalar);
                        Quaternion& operator/=(KernelTypes::Float Scalar);

                        Quaternion operator-() const;

                        KernelTypes::Bool operator==(const Quaternion& Other) const;
                        KernelTypes::Bool operator!=(const Quaternion& Other) const;
                    };

                    Quaternion operator*(KernelTypes::Float Scalar, const Quaternion& QuaternionValue);
                }
            }
        }
    }
}
