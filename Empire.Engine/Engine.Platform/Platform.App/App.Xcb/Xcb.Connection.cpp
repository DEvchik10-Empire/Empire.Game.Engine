#include "Empire.Engine/Engine.Platform/Platform.App/App.Xcb/Xcb.Connection.hpp"

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
                    namespace ConnectionXcb
                    {
                        Connection& Connection::Instance() noexcept
                        {
                            static Connection XcbConnection;
                            return XcbConnection;
                        }

                        Connection::Connection() noexcept
                        {
                            NativeConnection = xcb_connect(nullptr, nullptr);
                            if (NativeConnection == nullptr)
                            {
                                ErrorCode = -1;
                                return;
                            }

                            ErrorCode = xcb_connection_has_error(NativeConnection);
                            if (ErrorCode != 0)
                            {
                                Close();
                                return;
                            }

                            if (xcb_get_setup(NativeConnection) == nullptr)
                            {
                                ErrorCode = -1;
                                Close();
                            }
                        }

                        Connection::~Connection()
                        {
                            Close();
                        }

                        xcb_connection_t* Connection::Get() const noexcept
                        {
                            if (
                                NativeConnection == nullptr ||
                                xcb_connection_has_error(NativeConnection) != 0
                            )
                            {
                                return nullptr;
                            }

                            return NativeConnection;
                        }

                        Int32 Connection::GetErrorCode() const noexcept
                        {
                            if (NativeConnection != nullptr)
                            {
                                return xcb_connection_has_error(NativeConnection);
                            }

                            return ErrorCode;
                        }

                        void Connection::Close() noexcept
                        {
                            if (NativeConnection != nullptr)
                            {
                                xcb_disconnect(NativeConnection);
                                NativeConnection = nullptr;
                            }
                        }
                    }
                }
            }
        }
    }
}
