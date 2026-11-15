#include "EventQueue.hpp"

#include <cstdlib>

#include "Connection.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            namespace
            {
                xcb_window_t GetEventWindow(
                    const xcb_generic_event_t* event)
                {
                    if (!event)
                        return XCB_WINDOW_NONE;

                    const uint8_t responseType =
                        event->response_type & ~0x80;

                    switch (responseType)
                    {
                        case XCB_CLIENT_MESSAGE:
                            return reinterpret_cast<
                                const xcb_client_message_event_t*
                            >(event)->window;

                        case XCB_CONFIGURE_NOTIFY:
                            return reinterpret_cast<
                                const xcb_configure_notify_event_t*
                            >(event)->window;

                        case XCB_EXPOSE:
                            return reinterpret_cast<
                                const xcb_expose_event_t*
                            >(event)->window;

                        case XCB_KEY_PRESS:
                        case XCB_KEY_RELEASE:
                            return reinterpret_cast<
                                const xcb_key_press_event_t*
                            >(event)->event;

                        case XCB_BUTTON_PRESS:
                        case XCB_BUTTON_RELEASE:
                            return reinterpret_cast<
                                const xcb_button_press_event_t*
                            >(event)->event;

                        case XCB_MOTION_NOTIFY:
                            return reinterpret_cast<
                                const xcb_motion_notify_event_t*
                            >(event)->event;

                        case XCB_ENTER_NOTIFY:
                        case XCB_LEAVE_NOTIFY:
                            return reinterpret_cast<
                                const xcb_enter_notify_event_t*
                            >(event)->event;

                        case XCB_FOCUS_IN:
                        case XCB_FOCUS_OUT:
                            return reinterpret_cast<
                                const xcb_focus_in_event_t*
                            >(event)->event;

                        case XCB_DESTROY_NOTIFY:
                            return reinterpret_cast<
                                const xcb_destroy_notify_event_t*
                            >(event)->event;

                        case XCB_MAP_NOTIFY:
                            return reinterpret_cast<
                                const xcb_map_notify_event_t*
                            >(event)->window;

                        case XCB_UNMAP_NOTIFY:
                            return reinterpret_cast<
                                const xcb_unmap_notify_event_t*
                            >(event)->window;

                        case XCB_PROPERTY_NOTIFY:
                            return reinterpret_cast<
                                const xcb_property_notify_event_t*
                            >(event)->window;

                        default:
                            return XCB_WINDOW_NONE;
                    }
                }
            }


            EventQueue& EventQueue::Instance()
            {
                static EventQueue instance;
                return instance;
            }


            EventQueue::~EventQueue()
            {
                for (auto& [window, events] : pendingEvents)
                {
                    for (xcb_generic_event_t* event : events)
                        std::free(event);
                }

                pendingEvents.clear();
            }


            void EventQueue::RegisterWindow(
                xcb_window_t window)
            {
                pendingEvents.try_emplace(window);
            }


            void EventQueue::UnregisterWindow(
                xcb_window_t window)
            {
                auto it = pendingEvents.find(window);

                if (it == pendingEvents.end())
                    return;

                for (xcb_generic_event_t* event : it->second)
                    std::free(event);

                pendingEvents.erase(it);
            }


            xcb_generic_event_t* EventQueue::PollEvent(
                xcb_window_t window)
            {
                auto windowIt =
                    pendingEvents.find(window);

                if (windowIt == pendingEvents.end())
                    return nullptr;

                auto& events =
                    windowIt->second;

                if (!events.empty())
                {
                    xcb_generic_event_t* event =
                        events.front();

                    events.pop_front();

                    return event;
                }

                xcb_connection_t* conn =
                    Connection::Instance().get();

                xcb_generic_event_t* event = nullptr;

                while (
                    (event = xcb_poll_for_event(conn))
                )
                {
                    const xcb_window_t eventWindow =
                        GetEventWindow(event);

                    if (eventWindow == window)
                        return event;

                    auto otherWindow =
                        pendingEvents.find(eventWindow);

                    if (otherWindow != pendingEvents.end())
                    {
                        otherWindow->second.push_back(event);
                        continue;
                    }

                    std::free(event);
                }

                return nullptr;
            }


            xcb_generic_event_t* EventQueue::WaitEvent(
                xcb_window_t window)
            {
                auto windowIt =
                    pendingEvents.find(window);

                if (windowIt == pendingEvents.end())
                    return nullptr;

                auto& events =
                    windowIt->second;

                if (!events.empty())
                {
                    xcb_generic_event_t* event =
                        events.front();

                    events.pop_front();

                    return event;
                }

                xcb_connection_t* conn =
                    Connection::Instance().get();

                while (true)
                {
                    xcb_generic_event_t* event =
                        xcb_wait_for_event(conn);

                    if (!event)
                        return nullptr;

                    const xcb_window_t eventWindow =
                        GetEventWindow(event);

                    if (eventWindow == window)
                        return event;

                    auto otherWindow =
                        pendingEvents.find(eventWindow);

                    if (otherWindow != pendingEvents.end())
                    {
                        otherWindow->second.push_back(event);
                        continue;
                    }

                    std::free(event);
                }
            }
        }
    }
}