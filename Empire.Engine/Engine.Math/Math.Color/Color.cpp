// Adapted from Jolt Physics 5.7.0, Jolt/Core/Color.h and Color.cpp.
// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include "Empire.Engine/Engine.Math/Math.Color/Color.hpp"

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
                    const Color Color::Black(0, 0, 0);
                    const Color Color::DarkRed(128, 0, 0);
                    const Color Color::Red(255, 0, 0);
                    const Color Color::DarkGreen(0, 128, 0);
                    const Color Color::Green(0, 255, 0);
                    const Color Color::DarkBlue(0, 0, 128);
                    const Color Color::Blue(0, 0, 255);
                    const Color Color::Yellow(255, 255, 0);
                    const Color Color::Purple(255, 0, 255);
                    const Color Color::Cyan(0, 255, 255);
                    const Color Color::Orange(255, 128, 0);
                    const Color Color::DarkOrange(128, 64, 0);
                    const Color Color::Grey(128, 128, 128);
                    const Color Color::LightGrey(192, 192, 192);
                    const Color Color::White(255, 255, 255);

                    namespace
                    {
                        constexpr Color DistinctColors[] =
                        {
                            Color(255, 0, 0), Color(204, 143, 102), Color(226, 242, 0), Color(41, 166, 124),
                            Color(0, 170, 255), Color(69, 38, 153), Color(153, 38, 130), Color(229, 57, 80),
                            Color(204, 0, 0), Color(255, 170, 0), Color(85, 128, 0), Color(64, 255, 217),
                            Color(0, 75, 140), Color(161, 115, 230), Color(242, 61, 157), Color(178, 101, 89),
                            Color(140, 94, 0), Color(181, 217, 108), Color(64, 242, 255), Color(77, 117, 153),
                            Color(157, 61, 242), Color(140, 0, 56), Color(127, 57, 32), Color(204, 173, 51),
                            Color(64, 255, 64), Color(38, 145, 153), Color(0, 102, 255), Color(242, 0, 226),
                            Color(153, 77, 107), Color(229, 92, 0), Color(140, 126, 70), Color(0, 179, 71),
                            Color(0, 194, 242), Color(27, 0, 204), Color(230, 115, 222), Color(127, 0, 17)
                        };
                    }

                    Color Color::GetDistinctColor(Int32 Index)
                    {
                        EngineKernel::KernelAssert::DebugCheck(Index >= 0, "Color::GetDistinctColor: индекс должен быть неотрицательным");
                        const UInt32 SafeIndex = static_cast<UInt32>(Index);
                        return DistinctColors[SafeIndex % (sizeof(DistinctColors) / sizeof(DistinctColors[0]))];
                    }
                }
            }
        }
    }
}
