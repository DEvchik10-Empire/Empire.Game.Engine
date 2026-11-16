#include "Empire.Platform/Platform.App/App.Noop/Noop.App.hpp"

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
                    App& App::Instance() noexcept
                    {
                        static App NoopApp;
                        return NoopApp;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return true;
                    }

                    void* App::GetNativeHandle() const noexcept
                    {
                        return nullptr;
                    }

                    Int32 App::GetErrorCode() const noexcept
                    {
                        return 0;
                    }
                }
            }
        }
    }
}
