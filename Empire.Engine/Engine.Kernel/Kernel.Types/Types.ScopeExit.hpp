#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

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
                    // Вызывает функцию при выходе из области видимости (перенесён из Jolt, Core/ScopeExit.h).
                    // Использование: ScopeExit Guard([&] { код при выходе; });
                    // Несколько объектов в одной области вызываются в обратном порядке создания.
                    template <typename Function>
                    class ScopeExit : public NonCopyable
                    {
                        public:
                            explicit ScopeExit(Function&& ExitFunction)
                                : Callback(Move(ExitFunction))
                            {
                            }

                            ~ScopeExit()
                            {
                                if (!Invoked)
                                {
                                    Callback();
                                }
                            }

                            // Вызвать функцию сейчас, а не при выходе
                            void Invoke()
                            {
                                if (!Invoked)
                                {
                                    Callback();
                                    Invoked = true;
                                }
                            }

                            // Больше не вызывать функцию при выходе
                            void Release()
                            {
                                Invoked = true;
                            }

                        private:
                            Function Callback;
                            bool Invoked = false;
                    };
                }
            }
        }
    }
}
