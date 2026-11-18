#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.Quaternion.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathTransform
                {
                    namespace Transform3D
                    {
                        using namespace EngineKernel;
                        using MathMatrix::Matrix4;
                        using MathQuaternion::Quaternion;
                        using MathVector::Vector3;

                        struct Transform
                        {
                            Vector3 Position;
                            Quaternion Rotation;
                            Vector3 Scale;

                            Transform();

                            Transform(const Vector3& Position, const Quaternion& Rotation, const Vector3& Scale);

                            static Transform Identity();

                            Matrix4 ToMatrix4() const;

                            Vector3 TransformPoint(const Vector3& Point) const;
                            Vector3 TransformDirection(const Vector3& Direction) const;

                            Transform Inversed() const;

                            Transform operator*(const Transform& Other) const;

                            Transform& operator*=(const Transform& Other);

                            KernelTypes::Bool operator==(const Transform& Other) const;
                            KernelTypes::Bool operator!=(const Transform& Other) const;
                        };
                    }
                }
            }
        }
    }
}
