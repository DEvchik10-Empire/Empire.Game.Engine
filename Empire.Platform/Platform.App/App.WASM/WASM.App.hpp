#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Platform/Platform.ThirdParty/ThirdParty.WASM/WASM.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppWASM
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

                            // Browser pages have no native application object or window handle.
                            // Emscripten reports 1.0 when the browser has no device-pixel-ratio value.
                            [[nodiscard]] Double GetDevicePixelRatio() const noexcept;
                            // Changes document.title; returns false for a null title.
                            [[nodiscard]] Bool SetWindowTitle(const Char* Title) const noexcept;

                        private:
                            App() noexcept = default;
                            ~App() = default;
                    };
                }
            }
        }
    }
}
