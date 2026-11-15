#pragma once 

#include <xcb/xcb.h>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace XCB
                {
                    class App
                    {
                        private: 
                            xcb_connection_t* connection = nullptr;

                            App();
                            ~App();

                        public:
                            static App& Instance();

                            App(const App&) = delete;
                            App& operator=(const App&) = delete;

                            xcb_connection_t* get() const;
                    };
                }
            }
        }
    }
}