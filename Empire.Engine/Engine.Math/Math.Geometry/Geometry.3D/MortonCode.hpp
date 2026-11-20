#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace Geometry3D
                {
                    using namespace EngineKernel;
                    using namespace MathVector;

                    class MortonCode
                    {
                    public:
                        [[nodiscard]] static KernelTypes::UInt32 sExpandBits(KernelTypes::Float Value)
                        {
                            KernelAssert::DebugCheck(Value >= 0.0f && Value <= 1.0f,
                                "Morton coordinate must be in [0, 1]");
                            KernelTypes::UInt32 Expanded = static_cast<KernelTypes::UInt32>(Value * 1023.0f + 0.5f);
                            KernelAssert::DebugCheck(Expanded < 1024, "Morton coordinate exceeds 10 bits");
                            Expanded = (Expanded * 0x00010001u) & 0xFF0000FFu;
                            Expanded = (Expanded * 0x00000101u) & 0x0F00F00Fu;
                            Expanded = (Expanded * 0x00000011u) & 0xC30C30C3u;
                            Expanded = (Expanded * 0x00000005u) & 0x49249249u;
                            return Expanded;
                        }

                        [[nodiscard]] static KernelTypes::UInt32 sGetMortonCode(const Vector3& Value, const AABB& Bounds)
                        {
                            const Vector3 Scaled = (Value - Bounds.mMin) / Bounds.GetSize();
                            const KernelTypes::UInt32 X = sExpandBits(Scaled.GetX());
                            const KernelTypes::UInt32 Y = sExpandBits(Scaled.GetY());
                            const KernelTypes::UInt32 Z = sExpandBits(Scaled.GetZ());
                            return (X << 2) + (Y << 1) + Z;
                        }
                    };
                }
            }
        }
    }
}
