#pragma once

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelTypes
            {
                // Базовый класс, запрещающий копирование наследника (перенесён из Jolt, Core/NonCopyable.h).
                class NonCopyable
                {
                    public:
                        NonCopyable() = default;
                        NonCopyable(const NonCopyable&) = delete;
                        NonCopyable& operator=(const NonCopyable&) = delete;
                };
            }
        }
    }
}
