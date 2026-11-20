// Adapted from Jolt Physics 5.7.0, Jolt/Core/Color.h and Color.cpp.
// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathColor
                {
                    using namespace EngineKernel::KernelTypes;
                    using MathVector::Vector4;

                    // RGBA color with 8 bits per channel. Ported from Jolt Physics 5.7.0 Core/Color.h.
                    class [[nodiscard]] Color
                    {
                        public:
                            Color() = default;
                            Color(const Color&) = default;
                            Color& operator=(const Color&) = default;

                            explicit constexpr Color(UInt32 Packed) noexcept
                                : Channels{
                                    static_cast<UInt8>(Packed),
                                    static_cast<UInt8>(Packed >> 8),
                                    static_cast<UInt8>(Packed >> 16),
                                    static_cast<UInt8>(Packed >> 24)}
                            {
                            }

                            constexpr Color(UInt8 Red, UInt8 Green, UInt8 Blue, UInt8 Alpha = 255) noexcept
                                : Channels{Red, Green, Blue, Alpha}
                            {
                            }

                            constexpr Color(const Color& Other, UInt8 Alpha) noexcept
                                : Color(Other.GetRed(), Other.GetGreen(), Other.GetBlue(), Alpha)
                            {
                            }

                            constexpr Bool operator==(const Color& Other) const noexcept
                            {
                                return GetUInt32() == Other.GetUInt32();
                            }

                            constexpr Bool operator!=(const Color& Other) const noexcept
                            {
                                return GetUInt32() != Other.GetUInt32();
                            }

                            constexpr UInt32 GetUInt32() const noexcept
                            {
                                return static_cast<UInt32>(Channels[0])
                                    | (static_cast<UInt32>(Channels[1]) << 8)
                                    | (static_cast<UInt32>(Channels[2]) << 16)
                                    | (static_cast<UInt32>(Channels[3]) << 24);
                            }

                            constexpr UInt8 GetRed() const noexcept
                            {
                                return Channels[0];
                            }

                            constexpr UInt8 GetGreen() const noexcept
                            {
                                return Channels[1];
                            }

                            constexpr UInt8 GetBlue() const noexcept
                            {
                                return Channels[2];
                            }

                            constexpr UInt8 GetAlpha() const noexcept
                            {
                                return Channels[3];
                            }

                            constexpr UInt8 operator()(UInt32 Index) const noexcept
                            {
                                EngineKernel::KernelAssert::DebugCheck(Index < 4, "Color::operator(): индекс канала вне диапазона");
                                return Channels[Index];
                            }

                            UInt8& operator()(UInt32 Index) noexcept
                            {
                                EngineKernel::KernelAssert::DebugCheck(Index < 4, "Color::operator(): индекс канала вне диапазона");
                                return Channels[Index];
                            }

                            void SetChannel(UInt32 Index, UInt8 Value) noexcept
                            {
                                EngineKernel::KernelAssert::DebugCheck(Index < 4, "Color::SetChannel: индекс канала вне диапазона");
                                Channels[Index] = Value;
                            }

                            constexpr Color operator*(const Color& Other) const noexcept
                            {
                                return Color(
                                    static_cast<UInt8>((static_cast<UInt32>(GetRed()) * Other.GetRed()) >> 8),
                                    static_cast<UInt8>((static_cast<UInt32>(GetGreen()) * Other.GetGreen()) >> 8),
                                    static_cast<UInt8>((static_cast<UInt32>(GetBlue()) * Other.GetBlue()) >> 8),
                                    static_cast<UInt8>((static_cast<UInt32>(GetAlpha()) * Other.GetAlpha()) >> 8));
                            }

                            Color operator*(Float Intensity) const noexcept
                            {
                                return Color(
                                    static_cast<UInt8>(GetRed() * Intensity),
                                    static_cast<UInt8>(GetGreen() * Intensity),
                                    static_cast<UInt8>(GetBlue() * Intensity),
                                    GetAlpha());
                            }

                            Vector4 ToVec4() const noexcept
                            {
                                return Vector4(GetRed(), GetGreen(), GetBlue(), GetAlpha()) / 255.0f;
                            }

                            constexpr UInt8 GetIntensity() const noexcept
                            {
                                return static_cast<UInt8>((static_cast<UInt32>(GetRed()) * 54
                                    + static_cast<UInt32>(GetGreen()) * 183
                                    + static_cast<UInt32>(GetBlue()) * 19) >> 8);
                            }

                            static Color GetDistinctColor(Int32 Index);

                            static Color GreenRedGradient(Float Value) noexcept
                            {
                                if (Value < 0.0f)
                                {
                                    return Green;
                                }
                                if (Value < 0.5f)
                                {
                                    return Color(static_cast<UInt8>(510.0f * Value), 255, 0);
                                }
                                if (Value < 1.0f)
                                {
                                    return Color(255, static_cast<UInt8>(510.0f * (1.0f - Value)), 0);
                                }
                                return Red;
                            }

                            static const Color Black;
                            static const Color DarkRed;
                            static const Color Red;
                            static const Color DarkGreen;
                            static const Color Green;
                            static const Color DarkBlue;
                            static const Color Blue;
                            static const Color Yellow;
                            static const Color Purple;
                            static const Color Cyan;
                            static const Color Orange;
                            static const Color DarkOrange;
                            static const Color Grey;
                            static const Color LightGrey;
                            static const Color White;

                        private:
                            UInt8 Channels[4];
                    };

                    using ColorArg = Color;

                    constexpr Color operator*(Float Intensity, const Color& Value) noexcept
                    {
                        return Value * Intensity;
                    }

                    static_assert(sizeof(Color) == sizeof(UInt32));
                }
            }
        }
    }
}
