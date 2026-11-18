#include "Empire.Engine/Engine.Platform/Platform.App/App.Wayland/Wayland.Display.hpp"

#include <cerrno>
#include <poll.h>

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
                    namespace AppWayland
                    {
                        Display::Display() noexcept
                        {
                            errno = 0;
                            NativeDisplay = wl_display_connect(nullptr);

                            if (NativeDisplay == nullptr)
                                SetErrorCode();
                        }

                        Display::~Display()
                        {
                            if (NativeDisplay != nullptr)
                            {
                                wl_display_disconnect(NativeDisplay);
                                NativeDisplay = nullptr;
                            }
                        }

                        Display::Display(Display&& Other) noexcept
                            : NativeDisplay(Other.NativeDisplay),
                              ErrorCode(Other.ErrorCode)
                        {
                            Other.NativeDisplay = nullptr;
                            Other.ErrorCode = 0;
                        }

                        Bool Display::Dispatch() noexcept
                        {
                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            errno = 0;
                            if (wl_display_dispatch(NativeDisplay) == -1)
                            {
                                SetErrorCode();
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        Bool Display::DispatchPending() noexcept
                        {
                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            errno = 0;
                            if (wl_display_dispatch_pending(NativeDisplay) == -1)
                            {
                                SetErrorCode();
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        Bool Display::Roundtrip() noexcept
                        {
                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            errno = 0;
                            if (wl_display_roundtrip(NativeDisplay) == -1)
                            {
                                SetErrorCode();
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        Int32 Display::PumpEvents() noexcept
                        {
                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return -1;
                            }

                            Int32 HandledEvents = 0;

                            while (wl_display_prepare_read(NativeDisplay) != 0)
                            {
                                errno = 0;
                                const Int32 PendingEvents = wl_display_dispatch_pending(NativeDisplay);
                                if (PendingEvents == -1)
                                {
                                    SetErrorCode();
                                    return -1;
                                }

                                HandledEvents += PendingEvents;
                            }

                            errno = 0;
                            if (wl_display_flush(NativeDisplay) == -1 && errno != EAGAIN)
                            {
                                wl_display_cancel_read(NativeDisplay);
                                SetErrorCode();
                                return -1;
                            }

                            const Int32 DisplayFileDescriptor = wl_display_get_fd(NativeDisplay);
                            if (DisplayFileDescriptor == -1)
                            {
                                wl_display_cancel_read(NativeDisplay);
                                SetErrorCode();
                                return -1;
                            }

                            pollfd DisplayPollDescriptor{};
                            DisplayPollDescriptor.fd = DisplayFileDescriptor;
                            DisplayPollDescriptor.events = POLLIN;

                            Int32 PollResult = 0;
                            do
                            {
                                PollResult = poll(&DisplayPollDescriptor, 1, 0);
                            }
                            while (PollResult == -1 && errno == EINTR);

                            if (PollResult == -1)
                            {
                                wl_display_cancel_read(NativeDisplay);
                                SetErrorCode();
                                return -1;
                            }

                            if (PollResult > 0 && (DisplayPollDescriptor.revents & POLLIN) != 0)
                            {
                                errno = 0;
                                if (wl_display_read_events(NativeDisplay) == -1)
                                {
                                    SetErrorCode();
                                    return -1;
                                }
                            }
                            else
                            {
                                wl_display_cancel_read(NativeDisplay);

                                if (
                                    PollResult > 0 &&
                                    (DisplayPollDescriptor.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0
                                )
                                {
                                    ErrorCode = -1;
                                    return -1;
                                }
                            }

                            errno = 0;
                            const Int32 NewlyHandledEvents = wl_display_dispatch_pending(NativeDisplay);
                            if (NewlyHandledEvents == -1)
                            {
                                SetErrorCode();
                                return -1;
                            }

                            ErrorCode = 0;
                            return HandledEvents + NewlyHandledEvents;
                        }

                        Int32 Display::GetFd() const noexcept
                        {
                            if (NativeDisplay == nullptr)
                                return -1;

                            return wl_display_get_fd(NativeDisplay);
                        }

                        wl_display* Display::Get() const noexcept
                        {
                            return NativeDisplay;
                        }

                        Bool Display::IsReady() const noexcept
                        {
                            return NativeDisplay != nullptr &&
                                wl_display_get_error(NativeDisplay) == 0;
                        }

                        Int32 Display::GetErrorCode() const noexcept
                        {
                            if (NativeDisplay != nullptr)
                            {
                                const Int32 DisplayError = wl_display_get_error(NativeDisplay);
                                if (DisplayError != 0)
                                    return DisplayError;
                            }

                            return ErrorCode;
                        }

                        Bool Display::Flush() noexcept
                        {
                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            errno = 0;
                            if (wl_display_flush(NativeDisplay) == -1 && errno != EAGAIN)
                            {
                                SetErrorCode();
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        void Display::SetErrorCode() noexcept
                        {
                            if (NativeDisplay != nullptr)
                            {
                                const Int32 DisplayError = wl_display_get_error(NativeDisplay);
                                if (DisplayError != 0)
                                {
                                    ErrorCode = DisplayError;
                                    return;
                                }
                            }

                            ErrorCode = errno != 0 ? errno : -1;
                        }
                    }
                }
            }
        }
    }
}
