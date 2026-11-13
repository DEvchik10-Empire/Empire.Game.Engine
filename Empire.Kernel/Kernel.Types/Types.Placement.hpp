#pragma once

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Types
            {
                struct PlacementTag
                {
                };
            }
        }
    }
}

inline void* operator new(
    decltype(sizeof(0)),
    DEvchik10Empire::EmpireGameEngine::EngineKernel::Types::PlacementTag,
    void* Location
)
{
    return Location;
}

inline void operator delete(
    void*,
    DEvchik10Empire::EmpireGameEngine::EngineKernel::Types::PlacementTag,
    void*
) noexcept
{
}
