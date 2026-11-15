// Connection.cpp
#include "Connection.hpp"

#include <stdexcept>
#include <string>

namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            Connection& Connection::Instance()
            {
                static Connection instance;
                return instance;
            }

            Connection::Connection()
                : connection(xcb_connect(nullptr, nullptr))
            {
                const int errorCode =
                    xcb_connection_has_error(connection);

                if (errorCode != 0)
                {
                    xcb_disconnect(connection);
                    connection = nullptr;

                    throw std::runtime_error(
                        "xcb_connect failed, error code: "
                        + std::to_string(errorCode)
                    );
                }
            }

            Connection::~Connection()
            {
                if (connection)
                {
                    xcb_disconnect(connection);
                }
            }

            xcb_connection_t* Connection::get() const
            {
                return connection;
            }
        }
    }
}