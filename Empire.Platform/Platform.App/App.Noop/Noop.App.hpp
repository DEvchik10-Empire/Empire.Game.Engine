#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppNoop
                {
                    using namespace EmpireKernel::KernelTypes;

                    class App final
                    {
                        public:
                            [[nodiscard]] static App& Instance() noexcept;

                            App(const App&) = delete;
                            App& operator=(const App&) = delete;
                            App(App&&) = delete;
                            App& operator=(App&&) = delete;

                            [[nodiscard]] Bool IsReady() const noexcept;
                            [[nodiscard]] void* GetNativeHandle() const noexcept;
                            [[nodiscard]] Int32 GetErrorCode() const noexcept;

                        private:
                            App() noexcept = default;
                            ~App() = default;
                    };
                }
            }
        }
    }
}
