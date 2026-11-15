#pragma once 

#include "Empire.Platform/Platform.ThirdParty/ThirdParty.Legacy/Legacy.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace Legacy
                {
                    class Connection
                    {
                        public:
                            static Connection& Instance();

                            Connection(const Connection&) = delete;
                            Connection& operator=(const Connection&) = delete;

                            Connection(Connection&&) = delete;
                            Connection& operator=(Connection&&) = delete;

                            xcb_connection_t* get() const;

                        private:
                            Connection();
                            ~Connection();

                            xcb_connection_t* connection = nullptr;
                    };
                }
            }
        }
    }
}
