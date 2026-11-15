#pragma once

#include <deque>
#include <unordered_map>

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
                    class EventQueue
                    {
                    public:
                        static EventQueue& Instance();

                        EventQueue(const EventQueue&) = delete;
                        EventQueue& operator=(const EventQueue&) = delete;

                        EventQueue(EventQueue&&) = delete;
                        EventQueue& operator=(EventQueue&&) = delete;

                        void RegisterWindow(
                            xcb_window_t window
                        );

                        void UnregisterWindow(
                            xcb_window_t window
                        );

                        xcb_generic_event_t* PollEvent(
                            xcb_window_t window
                        );

                        xcb_generic_event_t* WaitEvent(
                            xcb_window_t window
                        );

                    private:
                        EventQueue() = default;
                        ~EventQueue();

                        std::unordered_map<
                            xcb_window_t,
                            std::deque<xcb_generic_event_t*>
                        > pendingEvents;
                    };
                }
            }
        }
    }
}