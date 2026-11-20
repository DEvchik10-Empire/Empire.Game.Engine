#pragma once

// Scalar Quat port from Jolt Physics 5.7.0 (Math/Quat.h); SIMD backends remain pending.
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Trigonometry.hpp"
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
                    using namespace EngineKernel;
                    using namespace MathVector;

                    class alignas(16) Quaternion
                    {
                    public:
                        constexpr Quaternion() noexcept : mValue(Vector4::sZero()) {}
                        constexpr Quaternion(KernelTypes::Float X, KernelTypes::Float Y,
                            KernelTypes::Float Z, KernelTypes::Float W) noexcept
                            : mValue(X, Y, Z, W)
                        {
                        }
                        explicit constexpr Quaternion(const Vector4& Value) noexcept : mValue(Value) {}
                        explicit Quaternion(const Float4& Value) noexcept : mValue(Vector4::sLoadFloat4(&Value)) {}

                        [[nodiscard]] KernelTypes::Bool operator==(const Quaternion& Other) const noexcept { return mValue == Other.mValue; }
                        [[nodiscard]] KernelTypes::Bool operator!=(const Quaternion& Other) const noexcept { return !(*this == Other); }
                        [[nodiscard]] KernelTypes::Bool IsClose(const Quaternion& Other, KernelTypes::Float MaxDistanceSquared = 1.0e-12f) const noexcept
                        {
                            return mValue.IsClose(Other.mValue, MaxDistanceSquared);
                        }
                        [[nodiscard]] KernelTypes::Bool IsNormalized(KernelTypes::Float Tolerance = 1.0e-5f) const noexcept
                        {
                            return mValue.IsNormalized(Tolerance);
                        }
                        [[nodiscard]] KernelTypes::Bool IsNaN() const noexcept { return mValue.IsNaN(); }

                        [[nodiscard]] constexpr KernelTypes::Float GetX() const noexcept { return mValue.GetX(); }
                        [[nodiscard]] constexpr KernelTypes::Float GetY() const noexcept { return mValue.GetY(); }
                        [[nodiscard]] constexpr KernelTypes::Float GetZ() const noexcept { return mValue.GetZ(); }
                        [[nodiscard]] constexpr KernelTypes::Float GetW() const noexcept { return mValue.GetW(); }
                        [[nodiscard]] constexpr Vector3 GetXYZ() const noexcept { return Vector3(mValue); }
                        [[nodiscard]] constexpr Vector4 GetXYZW() const noexcept { return mValue; }
                        constexpr void SetX(KernelTypes::Float Value) noexcept { mValue.SetX(Value); }
                        constexpr void SetY(KernelTypes::Float Value) noexcept { mValue.SetY(Value); }
                        constexpr void SetZ(KernelTypes::Float Value) noexcept { mValue.SetZ(Value); }
                        constexpr void SetW(KernelTypes::Float Value) noexcept { mValue.SetW(Value); }
                        constexpr void Set(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z, KernelTypes::Float W) noexcept
                        {
                            mValue.Set(X, Y, Z, W);
                        }

                        [[nodiscard]] static constexpr Quaternion sZero() noexcept { return {}; }
                        [[nodiscard]] static constexpr Quaternion sIdentity() noexcept { return {0, 0, 0, 1}; }
                        [[nodiscard]] static Quaternion sRotation(const Vector3& Axis, KernelTypes::Float Angle) noexcept;
                        void GetAxisAngle(Vector3& Axis, KernelTypes::Float& Angle) const noexcept;
                        [[nodiscard]] Vector3 GetAngularVelocity(KernelTypes::Float DeltaTime) const noexcept;
                        [[nodiscard]] static Quaternion sFromTo(const Vector3& From, const Vector3& To) noexcept;

                        template <typename Random>
                        [[nodiscard]] static Quaternion sRandom(Random& Generator) noexcept;

                        [[nodiscard]] static Quaternion sEulerAngles(const Vector3& Angles) noexcept;
                        [[nodiscard]] Vector3 GetEulerAngles() const noexcept;
                        [[nodiscard]] KernelTypes::Float LengthSq() const noexcept { return mValue.LengthSquared(); }
                        [[nodiscard]] KernelTypes::Float Length() const noexcept { return mValue.Length(); }
                        [[nodiscard]] Quaternion Normalized() const noexcept { return Quaternion(mValue.Normalized()); }
                        constexpr void operator+=(const Quaternion& Other) noexcept { mValue += Other.mValue; }
                        constexpr void operator-=(const Quaternion& Other) noexcept { mValue -= Other.mValue; }
                        constexpr void operator*=(KernelTypes::Float Value) noexcept { mValue *= Value; }
                        constexpr void operator/=(KernelTypes::Float Value) noexcept { mValue /= Value; }
                        [[nodiscard]] constexpr Quaternion operator-() const noexcept { return Quaternion(-mValue); }
                        [[nodiscard]] constexpr Quaternion operator+(const Quaternion& Other) const noexcept
                        {
                            return Quaternion(mValue + Other.mValue);
                        }
                        [[nodiscard]] constexpr Quaternion operator-(const Quaternion& Other) const noexcept
                        {
                            return Quaternion(mValue - Other.mValue);
                        }
                        [[nodiscard]] Quaternion operator*(const Quaternion& Other) const noexcept;
                        [[nodiscard]] constexpr Quaternion operator*(KernelTypes::Float Value) const noexcept
                        {
                            return Quaternion(mValue * Value);
                        }
                        [[nodiscard]] constexpr Quaternion operator/(KernelTypes::Float Value) const noexcept
                        {
                            return Quaternion(mValue / Value);
                        }
                        [[nodiscard]] Vector3 operator*(const Vector3& Value) const noexcept;
                        [[nodiscard]] static Quaternion sMultiplyImaginary(const Vector3& Left, const Quaternion& Right) noexcept;
                        [[nodiscard]] Vector3 InverseRotate(const Vector3& Value) const noexcept;
                        [[nodiscard]] Vector3 RotateAxisX() const noexcept { return *this * Vector3::sAxisX(); }
                        [[nodiscard]] Vector3 RotateAxisY() const noexcept { return *this * Vector3::sAxisY(); }
                        [[nodiscard]] Vector3 RotateAxisZ() const noexcept { return *this * Vector3::sAxisZ(); }
                        [[nodiscard]] KernelTypes::Float Dot(const Quaternion& Other) const noexcept { return mValue.Dot(Other.mValue); }
                        [[nodiscard]] constexpr Quaternion Conjugated() const noexcept
                        {
                            return Quaternion(mValue.FlipSign<-1, -1, -1, 1>());
                        }
                        [[nodiscard]] Quaternion Inversed() const noexcept { return Conjugated() / Length(); }
                        [[nodiscard]] Quaternion EnsureWPositive() const noexcept;
                        [[nodiscard]] Quaternion GetPerpendicular() const noexcept;
                        [[nodiscard]] KernelTypes::Float GetRotationAngle(const Vector3& Axis) const noexcept;
                        [[nodiscard]] Quaternion GetTwist(const Vector3& Axis) const noexcept;
                        void GetSwingTwist(Quaternion& Swing, Quaternion& Twist) const noexcept;
                        [[nodiscard]] Quaternion LERP(const Quaternion& Destination, KernelTypes::Float Fraction) const noexcept;
                        [[nodiscard]] Quaternion SLERP(const Quaternion& Destination, KernelTypes::Float Fraction) const noexcept;
                        [[nodiscard]] static Quaternion sLoadFloat3Unsafe(const Float3& Value) noexcept;
                        void StoreFloat3(Float3* Output) const noexcept;
                        void StoreFloat4(Float4* Output) const noexcept { mValue.StoreFloat4(Output); }
                        [[nodiscard]] KernelTypes::UInt32 CompressUnitQuat() const noexcept { return mValue.CompressUnitVector(); }
                        [[nodiscard]] static Quaternion sDecompressUnitQuat(KernelTypes::UInt32 Value) noexcept
                        {
                            return Quaternion(Vector4::sDecompressUnitVector(Value));
                        }

                    private:
                        [[nodiscard]] static Quaternion sRandomFromAngles(
                            KernelTypes::Float X0,
                            KernelTypes::Float Theta1,
                            KernelTypes::Float Theta2
                        ) noexcept;

                        Vector4 mValue;
                    };

                    static_assert(sizeof(Quaternion) == 16);
                    static_assert(alignof(Quaternion) == 16);

                    [[nodiscard]] constexpr Quaternion operator*(KernelTypes::Float Value, const Quaternion& Q) noexcept
                    {
                        return Q * Value;
                    }

                    template <typename Random>
                    Quaternion Quaternion::sRandom(Random& Generator) noexcept
                    {
                        const auto RandomUnit = [&Generator]() noexcept
                        {
                            return static_cast<KernelTypes::Float>(Generator() - Generator.min())
                                / static_cast<KernelTypes::Float>(Generator.max() - Generator.min());
                        };
                        const KernelTypes::Float X0 = RandomUnit();
                        const KernelTypes::Float Theta1 = MathScalar::TwoPi * RandomUnit();
                        const KernelTypes::Float Theta2 = MathScalar::TwoPi * RandomUnit();
                        return sRandomFromAngles(X0, Theta1, Theta2);
                    }
                }
            }
        }
    }
}
