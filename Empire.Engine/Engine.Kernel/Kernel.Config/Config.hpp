#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelConfig
                {
                    // Physics compatibility configuration, adapted from Jolt Physics 5.7.0 Core/Core.h.
                    // These are ordinary constexpr values: no feature-selection macros leak into engine code.
                    inline constexpr KernelTypes::UInt32 PhysicsVersionMajor = 5;
                    inline constexpr KernelTypes::UInt32 PhysicsVersionMinor = 7;
                    inline constexpr KernelTypes::UInt32 PhysicsVersionPatch = 0;

                    inline constexpr KernelTypes::Bool CrossPlatformDeterministic = false;
                    inline constexpr KernelTypes::Bool WorldUsesDoublePrecision = false;
                    inline constexpr KernelTypes::UInt32 ObjectLayerBits = 16;
                    inline constexpr KernelTypes::Bool AssertionsInRelease = false;
                    inline constexpr KernelTypes::Bool FloatingPointExceptions = false;
                    inline constexpr KernelTypes::Bool ProfileEnabled = false;
                    inline constexpr KernelTypes::Bool ExternalProfile = false;
                    inline constexpr KernelTypes::Bool DebugRendererEnabled = false;
                    inline constexpr KernelTypes::Bool DisableTempAllocator = false;
                    inline constexpr KernelTypes::Bool DisableCustomAllocator = false;
                    inline constexpr KernelTypes::Bool ObjectStreamEnabled = false;
                    // The port's numerical-behavior policy forbids fused multiply-add, independently of ISA support.
                    inline constexpr KernelTypes::Bool UseFMA = false;

                    inline constexpr KernelTypes::Bool AssertionsEnabled = KernelCompiler::IsDebugBuild || AssertionsInRelease;

                    template <KernelTypes::Bool UseDouble>
                    struct RealType;

                    template <>
                    struct RealType<false>
                    {
                        using Type = KernelTypes::Float;
                    };

                    template <>
                    struct RealType<true>
                    {
                        using Type = KernelTypes::Double;
                    };

                    using Real = typename RealType<WorldUsesDoublePrecision>::Type;

                    template <KernelTypes::UInt32 Bits>
                    struct ObjectLayerType;

                    template <>
                    struct ObjectLayerType<16>
                    {
                        using Type = KernelTypes::UInt16;
                    };

                    template <>
                    struct ObjectLayerType<32>
                    {
                        using Type = KernelTypes::UInt32;
                    };

                    template <>
                    struct ObjectLayerType<64>
                    {
                        using Type = KernelTypes::UInt64;
                    };

                    using ObjectLayer = typename ObjectLayerType<ObjectLayerBits>::Type;

                    inline constexpr KernelTypes::UInt64 VersionFeatureBits =
                        (static_cast<KernelTypes::UInt64>(WorldUsesDoublePrecision) << 0)
                        | (static_cast<KernelTypes::UInt64>(CrossPlatformDeterministic) << 1)
                        | (static_cast<KernelTypes::UInt64>(FloatingPointExceptions) << 2)
                        | (static_cast<KernelTypes::UInt64>(ProfileEnabled) << 3)
                        | (static_cast<KernelTypes::UInt64>(ExternalProfile) << 4)
                        | (static_cast<KernelTypes::UInt64>(DebugRendererEnabled) << 5)
                        | (static_cast<KernelTypes::UInt64>(DisableTempAllocator) << 6)
                        | (static_cast<KernelTypes::UInt64>(DisableCustomAllocator) << 7)
                        | (static_cast<KernelTypes::UInt64>(ObjectLayerBits == 64) << 8)
                        | (static_cast<KernelTypes::UInt64>(ObjectLayerBits == 32) << 9)
                        | (static_cast<KernelTypes::UInt64>(AssertionsEnabled) << 10)
                        | (static_cast<KernelTypes::UInt64>(ObjectStreamEnabled) << 11);

                    inline constexpr KernelTypes::UInt64 PhysicsVersionId =
                        (VersionFeatureBits << 24)
                        | (static_cast<KernelTypes::UInt64>(PhysicsVersionMajor) << 16)
                        | (static_cast<KernelTypes::UInt64>(PhysicsVersionMinor) << 8)
                        | PhysicsVersionPatch;

                    static_assert(ObjectLayerBits == 16 || ObjectLayerBits == 32 || ObjectLayerBits == 64,
                        "ObjectLayerBits must be 16, 32, or 64");
                }
            }
        }
    }
}
