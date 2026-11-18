#include "Empire.Engine/Engine.Platform/Platform.Surface/Surface.Xcb/Xcb.Surface.hpp"

#include "Empire.Engine/Engine.Platform/Platform.App/App.Xcb/Xcb.Connection.hpp"

#include <cstdlib>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformSurface
                {
                    namespace SurfaceXcb
                    {
                        Surface::~Surface()
                        {
                            DestroyWindow();
                        }

                        Bool Surface::Initialize(UInt16 Width, UInt16 Height) noexcept
                        {
                            if (Width == 0 || Height == 0 || Window != XCB_WINDOW_NONE)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            PlatformApp::ConnectionXcb::Connection& XcbConnection =
                                PlatformApp::ConnectionXcb::Connection::Instance();
                            if (XcbConnection.Get() == nullptr)
                            {
                                ErrorCode = XcbConnection.GetErrorCode();
                                if (ErrorCode == 0)
                                {
                                    ErrorCode = -1;
                                }
                                return false;
                            }

                            const xcb_setup_t* Setup = xcb_get_setup(XcbConnection.Get());
                            if (Setup == nullptr)
                            {
                                ErrorCode = xcb_connection_has_error(XcbConnection.Get());
                                if (ErrorCode == 0)
                                {
                                    ErrorCode = -1;
                                }
                                return false;
                            }

                            xcb_screen_iterator_t ScreenIterator = xcb_setup_roots_iterator(Setup);
                            if (ScreenIterator.rem == 0 || ScreenIterator.data == nullptr)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            const xcb_screen_t& Screen = *ScreenIterator.data;
                            Window = xcb_generate_id(XcbConnection.Get());
                            if (Window == XCB_WINDOW_NONE)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            const UInt32 WindowValues[] = { Screen.black_pixel };
                            const xcb_void_cookie_t CreateCookie = xcb_create_window_checked(
                                XcbConnection.Get(),
                                XCB_COPY_FROM_PARENT,
                                Window,
                                Screen.root,
                                0,
                                0,
                                Width,
                                Height,
                                0,
                                XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                Screen.root_visual,
                                XCB_CW_BACK_PIXEL,
                                WindowValues
                            );

                            xcb_generic_error_t* XcbError = xcb_request_check(
                                XcbConnection.Get(),
                                CreateCookie
                            );
                            if (XcbError != nullptr)
                            {
                                ErrorCode = static_cast<Int32>(XcbError->error_code);
                                std::free(XcbError);
                                Window = XCB_WINDOW_NONE;
                                return false;
                            }

                            const Char WindowTitle[] = "Empire Game Engine";
                            const xcb_void_cookie_t TitleCookie = xcb_change_property_checked(
                                XcbConnection.Get(),
                                XCB_PROP_MODE_REPLACE,
                                Window,
                                XCB_ATOM_WM_NAME,
                                XCB_ATOM_STRING,
                                8,
                                static_cast<UInt32>(sizeof(WindowTitle) - 1),
                                WindowTitle
                            );

                            XcbError = xcb_request_check(XcbConnection.Get(), TitleCookie);
                            if (XcbError != nullptr)
                            {
                                ErrorCode = static_cast<Int32>(XcbError->error_code);
                                std::free(XcbError);
                                DestroyWindow();
                                return false;
                            }

                            const xcb_void_cookie_t MapCookie = xcb_map_window_checked(
                                XcbConnection.Get(),
                                Window
                            );
                            XcbError = xcb_request_check(XcbConnection.Get(), MapCookie);
                            if (XcbError != nullptr)
                            {
                                ErrorCode = static_cast<Int32>(XcbError->error_code);
                                std::free(XcbError);
                                DestroyWindow();
                                return false;
                            }

                            if (xcb_flush(XcbConnection.Get()) <= 0)
                            {
                                ErrorCode = xcb_connection_has_error(XcbConnection.Get());
                                if (ErrorCode == 0)
                                {
                                    ErrorCode = -1;
                                }
                                DestroyWindow();
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        void Surface::Shutdown() noexcept
                        {
                            DestroyWindow();
                            ErrorCode = 0;
                        }

                        xcb_window_t Surface::GetWindow() const noexcept
                        {
                            return Window;
                        }

                        Int32 Surface::GetErrorCode() const noexcept
                        {
                            if (PlatformApp::ConnectionXcb::Connection::Instance().Get() != nullptr)
                            {
                                const Int32 ConnectionError = xcb_connection_has_error(
                                    PlatformApp::ConnectionXcb::Connection::Instance().Get()
                                );
                                if (ConnectionError != 0)
                                {
                                    return ConnectionError;
                                }
                            }

                            return ErrorCode;
                        }

                        void Surface::DestroyWindow() noexcept
                        {
                            if (Window == XCB_WINDOW_NONE)
                            {
                                return;
                            }

                            if (PlatformApp::ConnectionXcb::Connection::Instance().Get() != nullptr)
                            {
                                xcb_destroy_window(
                                    PlatformApp::ConnectionXcb::Connection::Instance().Get(),
                                    Window
                                );
                                (void)xcb_flush(PlatformApp::ConnectionXcb::Connection::Instance().Get());
                            }

                            Window = XCB_WINDOW_NONE;
                        }
                    }
                }
            }
        }
    }
}
