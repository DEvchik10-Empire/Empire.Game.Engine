#pragma once

#include <xcb/xcb.h>

namespace Engine
{
    namespace Platform
    {
        namespace legasy
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