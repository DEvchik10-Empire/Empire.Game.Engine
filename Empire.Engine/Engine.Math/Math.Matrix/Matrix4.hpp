#pragma once

// Scalar Mat44 port from Jolt Physics 5.7.0 (Math/Mat44.h); SIMD backends remain pending.
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

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
                    class Quaternion;
                }

                namespace MathMatrix
                {
                    using namespace EngineKernel;
                    using namespace MathVector;
                    using MathQuaternion::Quaternion;

                    class alignas(16) Matrix4
                    {
                    public:
                        constexpr Matrix4() noexcept
                            : mColumns{Vector4::sZero(), Vector4::sZero(), Vector4::sZero(), Vector4::sZero()}
                        {
                        }

                        constexpr Matrix4(const Vector4& C0, const Vector4& C1, const Vector4& C2, const Vector4& C3) noexcept
                            : mColumns{C0, C1, C2, C3}
                        {
                        }

                        constexpr Matrix4(const Vector4& C0, const Vector4& C1, const Vector4& C2, const Vector3& C3) noexcept
                            : mColumns{C0, C1, C2, Vector4(C3, 1.0f)}
                        {
                        }

                        [[nodiscard]] static constexpr Matrix4 sZero() noexcept { return {}; }
                        [[nodiscard]] static constexpr Matrix4 sIdentity() noexcept
                        {
                            return {
                                Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0),
                                Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1)
                            };
                        }
                        [[nodiscard]] static Matrix4 sNaN() noexcept;
                        [[nodiscard]] static Matrix4 sLoadFloat4x4(const Float4* Columns) noexcept;
                        [[nodiscard]] static Matrix4 sLoadFloat4x4Aligned(const Float4* Columns) noexcept;
                        [[nodiscard]] static Matrix4 sRotationX(KernelTypes::Float Angle) noexcept;
                        [[nodiscard]] static Matrix4 sRotationY(KernelTypes::Float Angle) noexcept;
                        [[nodiscard]] static Matrix4 sRotationZ(KernelTypes::Float Angle) noexcept;
                        [[nodiscard]] static Matrix4 sRotation(const Vector3& Axis, KernelTypes::Float Angle) noexcept;
                        [[nodiscard]] static Matrix4 sRotation(const Quaternion& Rotation) noexcept;
                        [[nodiscard]] static Matrix4 sTranslation(const Vector3& Translation) noexcept;
                        [[nodiscard]] static Matrix4 sRotationTranslation(const Quaternion& Rotation, const Vector3& Translation) noexcept;
                        [[nodiscard]] static Matrix4 sInverseRotationTranslation(const Quaternion& Rotation, const Vector3& Translation) noexcept;
                        [[nodiscard]] static Matrix4 sScale(KernelTypes::Float Scale) noexcept;
                        [[nodiscard]] static Matrix4 sScale(const Vector3& Scale) noexcept;
                        [[nodiscard]] static Matrix4 sOuterProduct(const Vector3& A, const Vector3& B) noexcept;
                        [[nodiscard]] static Matrix4 sCrossProduct(const Vector3& Value) noexcept;
                        [[nodiscard]] static Matrix4 sQuatLeftMultiply(const Quaternion& Value) noexcept;
                        [[nodiscard]] static Matrix4 sQuatRightMultiply(const Quaternion& Value) noexcept;
                        [[nodiscard]] static Matrix4 sLookAt(const Vector3& Position, const Vector3& Target, const Vector3& Up) noexcept;
                        [[nodiscard]] static Matrix4 sPerspective(KernelTypes::Float FovY, KernelTypes::Float Aspect,
                            KernelTypes::Float Near, KernelTypes::Float Far) noexcept;
                        [[nodiscard]] static Matrix4 sOrthographic(KernelTypes::Float Left, KernelTypes::Float Right,
                            KernelTypes::Float Bottom, KernelTypes::Float Top, KernelTypes::Float Near, KernelTypes::Float Far) noexcept;

                        [[nodiscard]] constexpr KernelTypes::Float operator()(KernelTypes::UInt32 Row, KernelTypes::UInt32 Column) const noexcept
                        {
                            KernelAssert::DebugCheck(Row < 4 && Column < 4, "Matrix4 indices are in range");
                            return mColumns[Column][Row];
                        }
                        constexpr KernelTypes::Float& operator()(KernelTypes::UInt32 Row, KernelTypes::UInt32 Column) noexcept
                        {
                            KernelAssert::DebugCheck(Row < 4 && Column < 4, "Matrix4 indices are in range");
                            return mColumns[Column][Row];
                        }
                        [[nodiscard]] KernelTypes::Bool operator==(const Matrix4& Other) const noexcept;
                        [[nodiscard]] KernelTypes::Bool operator!=(const Matrix4& Other) const noexcept { return !(*this == Other); }
                        [[nodiscard]] KernelTypes::Bool IsClose(const Matrix4& Other, KernelTypes::Float MaxDistanceSquared = 1.0e-12f) const noexcept;
                        [[nodiscard]] Matrix4 operator*(const Matrix4& Other) const noexcept;
                        [[nodiscard]] Vector3 operator*(const Vector3& Value) const noexcept;
                        [[nodiscard]] Vector4 operator*(const Vector4& Value) const noexcept;
                        [[nodiscard]] Vector3 Multiply3x3(const Vector3& Value) const noexcept;
                        [[nodiscard]] Vector3 Multiply3x3Transposed(const Vector3& Value) const noexcept;
                        [[nodiscard]] Matrix4 Multiply3x3(const Matrix4& Other) const noexcept;
                        [[nodiscard]] Matrix4 Multiply3x3LeftTransposed(const Matrix4& Other) const noexcept;
                        [[nodiscard]] Matrix4 Multiply3x3RightTransposed(const Matrix4& Other) const noexcept;
                        [[nodiscard]] Matrix4 operator*(KernelTypes::Float Scale) const noexcept;
                        [[nodiscard]] Matrix4 operator+(const Matrix4& Other) const noexcept;
                        [[nodiscard]] Matrix4 operator-() const noexcept;
                        [[nodiscard]] Matrix4 operator-(const Matrix4& Other) const noexcept;
                        Matrix4& operator+=(const Matrix4& Other) noexcept;
                        Matrix4& operator*=(KernelTypes::Float Scale) noexcept;

                        [[nodiscard]] Vector3 GetAxisX() const noexcept { return Vector3(mColumns[0]); }
                        void SetAxisX(const Vector3& Value) noexcept { mColumns[0] = Vector4(Value, 0.0f); }
                        [[nodiscard]] Vector3 GetAxisY() const noexcept { return Vector3(mColumns[1]); }
                        void SetAxisY(const Vector3& Value) noexcept { mColumns[1] = Vector4(Value, 0.0f); }
                        [[nodiscard]] Vector3 GetAxisZ() const noexcept { return Vector3(mColumns[2]); }
                        void SetAxisZ(const Vector3& Value) noexcept { mColumns[2] = Vector4(Value, 0.0f); }
                        [[nodiscard]] Vector3 GetTranslation() const noexcept { return Vector3(mColumns[3]); }
                        void SetTranslation(const Vector3& Value) noexcept { mColumns[3] = Vector4(Value, 1.0f); }
                        [[nodiscard]] Vector3 GetDiagonal3() const noexcept { return {mColumns[0].GetX(), mColumns[1].GetY(), mColumns[2].GetZ()}; }
                        void SetDiagonal3(const Vector3& Value) noexcept
                        {
                            mColumns[0].SetX(Value.GetX());
                            mColumns[1].SetY(Value.GetY());
                            mColumns[2].SetZ(Value.GetZ());
                        }
                        [[nodiscard]] Vector4 GetDiagonal4() const noexcept
                        {
                            return {mColumns[0].GetX(), mColumns[1].GetY(), mColumns[2].GetZ(), mColumns[3].GetW()};
                        }
                        void SetDiagonal4(const Vector4& Value) noexcept
                        {
                            mColumns[0].SetX(Value.GetX());
                            mColumns[1].SetY(Value.GetY());
                            mColumns[2].SetZ(Value.GetZ());
                            mColumns[3].SetW(Value.GetW());
                        }
                        [[nodiscard]] Vector3 GetColumn3(KernelTypes::UInt32 Column) const noexcept;
                        void SetColumn3(KernelTypes::UInt32 Column, const Vector3& Value) noexcept;
                        [[nodiscard]] Vector4 GetColumn4(KernelTypes::UInt32 Column) const noexcept;
                        void SetColumn4(KernelTypes::UInt32 Column, const Vector4& Value) noexcept;
                        void StoreFloat4x4(Float4* Output) const noexcept;
                        [[nodiscard]] Matrix4 Transposed() const noexcept;
                        [[nodiscard]] Matrix4 Transposed3x3() const noexcept;
                        [[nodiscard]] Matrix4 Inversed() const noexcept;
                        [[nodiscard]] Matrix4 InversedRotationTranslation() const noexcept;
                        [[nodiscard]] KernelTypes::Float GetDeterminant3x3() const noexcept;
                        [[nodiscard]] Matrix4 Adjointed3x3() const noexcept;
                        [[nodiscard]] Matrix4 Inversed3x3() const noexcept;
                        [[nodiscard]] KernelTypes::Bool SetInversed3x3(const Matrix4& Value) noexcept;
                        [[nodiscard]] Matrix4 GetRotation() const noexcept;
                        [[nodiscard]] Matrix4 GetRotationSafe() const noexcept;
                        void SetRotation(const Matrix4& Rotation) noexcept;
                        [[nodiscard]] Quaternion GetQuaternion() const noexcept;
                        [[nodiscard]] Matrix4 GetDirectionPreservingMatrix() const noexcept;
                        [[nodiscard]] Matrix4 PreTranslated(const Vector3& Translation) const noexcept;
                        [[nodiscard]] Matrix4 PostTranslated(const Vector3& Translation) const noexcept;
                        [[nodiscard]] Matrix4 PreScaled(const Vector3& Scale) const noexcept;
                        [[nodiscard]] Matrix4 PostScaled(const Vector3& Scale) const noexcept;
                        [[nodiscard]] Matrix4 Decompose(Vector3& Scale) const noexcept;

                        [[nodiscard]] static Matrix4 Perspective(KernelTypes::Float FovY, KernelTypes::Float Aspect,
                            KernelTypes::Float Near, KernelTypes::Float Far) noexcept
                        {
                            return sPerspective(FovY, Aspect, Near, Far);
                        }

                    private:
                        Vector4 mColumns[4];
                    };

                    static_assert(sizeof(Matrix4) == 64);
                    static_assert(alignof(Matrix4) == 16);

                    [[nodiscard]] inline Matrix4 operator*(KernelTypes::Float Scale, const Matrix4& Value) noexcept
                    {
                        return Value * Scale;
                    }
                }
            }
        }
    }
}
