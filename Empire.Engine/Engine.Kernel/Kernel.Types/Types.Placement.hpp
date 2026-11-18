#pragma once

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelTypes
                {
                    struct PlacementTag
                    {
                    };
                }
            }
        }
    }
}

inline void* operator new(
    decltype(sizeof(0)),
    DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes::PlacementTag,
    void* Location
)
{
    return Location;
}

inline void operator delete(
    void*,
    DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes::PlacementTag,
    void*
) noexcept
{
}
