#include "Empire.Engine/Engine.Platform/Platform.App/App.WASM/WASM.App.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformApp
                {
                    namespace AppWASM
                    {
                        App& App::Instance() noexcept
                        {
                            static App WASMApp;
                            return WASMApp;
                        }

                        Double App::GetDevicePixelRatio() const noexcept
                        {
                            return emscripten_get_device_pixel_ratio();
                        }

                        Bool App::SetWindowTitle(const Char* Title) const noexcept
                        {
                            if (Title == nullptr)
                                return false;

                            emscripten_set_window_title(Title);
                            return true;
                        }
                    }
                }
            }
        }
    }
}
