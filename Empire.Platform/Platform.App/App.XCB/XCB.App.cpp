#include "XCB.App.hpp"

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include <xcb/xcb.h>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace XCB
                {
                    
                    App& App::Instance()
                    {
                        static App instance;
                        return instance;
                    }

                    App::App()
                        : connection(xcb_connect(nullptr, nullptr))
                    {
                        const int ErrorCode = xcb_connection_has_error(connection);

                        if (ErrorCode != 0)
                        {
                            xcb_disconnect(connection);
                            connection = nullptr;

                            throw Error("Connection error code: " + ToString(ErrorCode)); 
                        }
                    }

                    App::~App()
                    {
                        xcb_disconnect(connection);
                    }
                }
            }
        }
    }
}