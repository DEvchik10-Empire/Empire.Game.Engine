#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.Reference.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    // Сопоставление объект/ID для потоковой сериализации (перенесено из Jolt 5.7.0, Core/ObjectToIDMap.h).
                    template <typename T>
                    using ObjectToIDMap = HashMap<const T*, KernelTypes::UInt32>;

                    template <typename T>
                    using IdToObjectMap = Array<KernelMemory::Ref<T>>;

                    // Точное имя из Jolt; сохраняем PascalCase-синоним выше для стиля Empire.
                    template <typename T>
                    using IDToObjectMap = IdToObjectMap<T>;
                }
            }
        }
    }
}
