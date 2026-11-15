#include "Window.hpp"
 
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <xcb/xcb.h>
 
#include "Empire.App/legasy/Connection.hpp"
#include "Empire.App/legasy/EventQueue.hpp"
//#include "WindowConfig.hpp"
#include "Kernel/Types.hpp"
 
namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            namespace
            {
                // Все атомы, которые нужны окну.
                // Атомы привязаны к соединению, а не к окну, поэтому запрашиваем
                // их ОДИН раз на всё приложение (а не на каждое окно / событие).
                struct Atoms
                {
                    xcb_atom_t wmProtocols          = XCB_ATOM_NONE;
                    xcb_atom_t wmDeleteWindow       = XCB_ATOM_NONE;
                    xcb_atom_t wmChangeState        = XCB_ATOM_NONE;
                    xcb_atom_t netWmName            = XCB_ATOM_NONE;
                    xcb_atom_t utf8String           = XCB_ATOM_NONE;
                    xcb_atom_t netWmState           = XCB_ATOM_NONE;
                    xcb_atom_t netWmStateFullscreen = XCB_ATOM_NONE;
                    xcb_atom_t netWmStateAbove      = XCB_ATOM_NONE;
                    xcb_atom_t netWmStateMaxVert    = XCB_ATOM_NONE;
                    xcb_atom_t netWmStateMaxHorz    = XCB_ATOM_NONE;
                };
 
                const Atoms& GetAtoms(
                    xcb_connection_t* conn)
                {
                    static const Atoms atoms = [conn]()
                    {
                        Atoms result;
 
                        struct Entry
                        {
                            const char* name;
                            xcb_atom_t* target;
                        };
 
                        const Entry entries[] = {
                            { "WM_PROTOCOLS",                  &result.wmProtocols },
                            { "WM_DELETE_WINDOW",              &result.wmDeleteWindow },
                            { "WM_CHANGE_STATE",               &result.wmChangeState },
                            { "_NET_WM_NAME",                  &result.netWmName },
                            { "UTF8_STRING",                   &result.utf8String },
                            { "_NET_WM_STATE",                 &result.netWmState },
                            { "_NET_WM_STATE_FULLSCREEN",      &result.netWmStateFullscreen },
                            { "_NET_WM_STATE_ABOVE",           &result.netWmStateAbove },
                            { "_NET_WM_STATE_MAXIMIZED_VERT",  &result.netWmStateMaxVert },
                            { "_NET_WM_STATE_MAXIMIZED_HORZ",  &result.netWmStateMaxHorz },
                        };
 
                        constexpr std::size_t count =
                            sizeof(entries) / sizeof(entries[0]);
 
                        xcb_intern_atom_cookie_t cookies[count];
 
                        // Сначала отправляем ВСЕ запросы...
                        for (std::size_t i = 0; i < count; ++i)
                        {
                            // Длина считается через strlen, руками не считаем
                            // (раньше тут были ошибки в длинах и атомы были неправильные).
                            cookies[i] =
                                xcb_intern_atom(
                                    conn,
                                    0,
                                    static_cast<uint16_t>(
                                        std::strlen(entries[i].name)),
                                    entries[i].name);
                        }
 
                        // ...и только потом собираем ответы (один раунд-трип вместо десяти).
                        for (std::size_t i = 0; i < count; ++i)
                        {
                            xcb_intern_atom_reply_t* reply =
                                xcb_intern_atom_reply(
                                    conn,
                                    cookies[i],
                                    nullptr);
 
                            if (reply)
                            {
                                *entries[i].target = reply->atom;
                                std::free(reply);
                            }
                        }
 
                        return result;
                    }();
 
                    return atoms;
                }
 
                xcb_window_t GetRootWindow(
                    xcb_connection_t* conn)
                {
                    return xcb_setup_roots_iterator(
                               xcb_get_setup(conn)).data->root;
                }
 
                // Клиентское сообщение корневому окну (так общаются с WM).
                void SendRootClientMessage(
                    xcb_connection_t* conn,
                    xcb_window_t window,
                    xcb_atom_t type,
                    Types::u32 d0,
                    Types::u32 d1 = 0,
                    Types::u32 d2 = 0,
                    Types::u32 d3 = 0)
                {
                    xcb_client_message_event_t event = {};
                    event.response_type = XCB_CLIENT_MESSAGE;
                    event.format = 32;
                    event.window = window;
                    event.type = type;
                    event.data.data32[0] = d0;
                    event.data.data32[1] = d1;
                    event.data.data32[2] = d2;
                    event.data.data32[3] = d3;
 
                    xcb_send_event(
                        conn,
                        0,
                        GetRootWindow(conn),
                        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
                        reinterpret_cast<const char*>(&event));
 
                    xcb_flush(conn);
                }
 
                // action: 0 = remove, 1 = add, 2 = toggle
                void SendNetWmStateMessage(
                    xcb_connection_t* conn,
                    xcb_window_t window,
                    Types::i32 action,
                    xcb_atom_t state1,
                    xcb_atom_t state2 = 0)
                {
                    const Atoms& atoms = GetAtoms(conn);
 
                    if (!atoms.netWmState)
                        return;
 
                    SendRootClientMessage(
                        conn,
                        window,
                        atoms.netWmState,
                        static_cast<Types::u32>(action),
                        state1,
                        state2,
                        1); // source indication: обычное приложение
                }
 
                Types::b8 NetWmStateContains(
                    xcb_connection_t* conn,
                    xcb_window_t window,
                    xcb_atom_t targetAtom)
                {
                    const Atoms& atoms = GetAtoms(conn);
 
                    if (!atoms.netWmState || !targetAtom)
                        return false;
 
                    xcb_get_property_cookie_t propCookie =
                        xcb_get_property(
                            conn,
                            0,
                            window,
                            atoms.netWmState,
                            XCB_ATOM_ATOM,
                            0,
                            1024);
 
                    xcb_get_property_reply_t* propReply =
                        xcb_get_property_reply(
                            conn,
                            propCookie,
                            nullptr);
 
                    if (!propReply)
                        return false;
 
                    Types::b8 found = false;
 
                    const xcb_atom_t* values =
                        static_cast<const xcb_atom_t*>(
                            xcb_get_property_value(propReply));
 
                    const Types::i32 count =
                        xcb_get_property_value_length(propReply) /
                        sizeof(xcb_atom_t);
 
                    for (Types::i32 i = 0; i < count; ++i)
                    {
                        if (values[i] == targetAtom)
                        {
                            found = true;
                            break;
                        }
                    }
 
                    std::free(propReply);
 
                    return found;
                }
            }
 
            Window::Window(const WindowConfig& config)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                window = xcb_generate_id(conn);
 
                const xcb_setup_t* setup =
                    xcb_get_setup(conn);
 
                xcb_screen_t* screen =
                    xcb_setup_roots_iterator(setup).data;
 
                Types::i32 actualWidth =
                    (config.width == -1)
                        ? screen->width_in_pixels
                        : config.width;
 
                Types::i32 actualHeight =
                    (config.height == -1)
                        ? screen->height_in_pixels
                        : config.height;
 
                const Types::u32 eventMask =
                    XCB_EVENT_MASK_PROPERTY_CHANGE |
                    XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                    XCB_EVENT_MASK_FOCUS_CHANGE;
 
                // Без BACK_PIXEL у окна фон = None: сервер его не закрашивает,
                // у окна нет ни одного пикселя, и XWayland/композитор его не показывает.
                // ВАЖНО: порядок значений = порядок битов маски по возрастанию
                // (BACK_PIXEL = 0x2, EVENT_MASK = 0x800).
                const Types::u32 valueMask =
                    XCB_CW_BACK_PIXEL |
                    XCB_CW_EVENT_MASK;
 
                const Types::u32 valueList[] = {
                    screen->black_pixel,
                    eventMask
                };
 
                xcb_create_window(
                    conn,
                    XCB_COPY_FROM_PARENT,
                    window,
                    screen->root,
                    0,
                    0,
                    actualWidth,
                    actualHeight,
                    0, // border_width (раньше было 10 -> рамка 10px)
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual,
                    valueMask,
                    valueList);
 
                // Заголовок (старый способ)
                xcb_change_property(
                    conn,
                    XCB_PROP_MODE_REPLACE,
                    window,
                    XCB_ATOM_WM_NAME,
                    XCB_ATOM_STRING,
                    8,
                    config.title.size(),
                    config.title.c_str());
 
                // Заголовок (UTF-8)
                netWmNameAtom = atoms.netWmName;
                utf8StringAtom = atoms.utf8String;
 
                if (netWmNameAtom && utf8StringAtom)
                {
                    xcb_change_property(
                        conn,
                        XCB_PROP_MODE_REPLACE,
                        window,
                        netWmNameAtom,
                        utf8StringAtom,
                        8,
                        config.title.size(),
                        config.title.c_str());
                }
 
                // Закрытие по крестику (WM_DELETE_WINDOW)
                protocolsAtom = atoms.wmProtocols;
                deleteAtom = atoms.wmDeleteWindow;
 
                if (protocolsAtom && deleteAtom)
                {
                    xcb_change_property(
                        conn,
                        XCB_PROP_MODE_REPLACE,
                        window,
                        protocolsAtom,
                        XCB_ATOM_ATOM,
                        32,
                        1,
                        &deleteAtom);
                }
 
                EventQueue::Instance().RegisterWindow(window);
 
                xcb_map_window(
                    conn,
                    window);
 
                xcb_flush(conn);
            }
 
            Window::~Window()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                EventQueue::Instance().UnregisterWindow(window);
 
                if (window != 0)
                {
                    xcb_destroy_window(
                        conn,
                        window);
 
                    xcb_flush(conn);
                }
            }
 
            xcb_window_t Window::Get() const
            {
                return window;
            }
 
            // Возвращает true, если событие было ИНТЕРЕСНЫМ для окна.
            // На логику цикла событий это влиять не должно (см. PollEvent).
            Types::b8 Window::HandleEvent(
                const xcb_generic_event_t* event)
            {
                if (!event)
                    return false;
 
                const Types::u8 responseType =
                    event->response_type & ~0x80;
 
                if (responseType == XCB_CONFIGURE_NOTIFY)
                {
                    const auto* configureEvent =
                        reinterpret_cast<
                            const xcb_configure_notify_event_t*>(
                            event);
 
                    if (configureEvent->width != lastKnownWidth ||
                        configureEvent->height != lastKnownHeight)
                    {
                        resizedFlag = true;
 
                        lastKnownWidth =
                            configureEvent->width;
 
                        lastKnownHeight =
                            configureEvent->height;
                    }
 
                    if (configureEvent->x != lastKnownX ||
                        configureEvent->y != lastKnownY)
                    {
                        movedFlag = true;
 
                        lastKnownX =
                            configureEvent->x;
 
                        lastKnownY =
                            configureEvent->y;
                    }
 
                    return true;
                }
 
                if (responseType == XCB_FOCUS_IN)
                {
                    hasFocus = true;
                    return true;
                }
 
                if (responseType == XCB_FOCUS_OUT)
                {
                    hasFocus = false;
                    return true;
                }
 
                if (responseType == XCB_PROPERTY_NOTIFY)
                {
                    const auto* propertyEvent =
                        reinterpret_cast<
                            const xcb_property_notify_event_t*>(
                            event);
 
                    xcb_connection_t* conn =
                        Connection::Instance().get();
 
                    const Atoms& atoms =
                        GetAtoms(conn);
 
                    // Перепроверяем состояние ТОЛЬКО если поменялся _NET_WM_STATE,
                    // а не на каждый чих (раньше тут было 3 синхронных запроса на любое свойство).
                    if (propertyEvent->atom == atoms.netWmState)
                    {
                        isFullscreen =
                            NetWmStateContains(
                                conn,
                                window,
                                atoms.netWmStateFullscreen);
 
                        isAlwaysOnTop =
                            NetWmStateContains(
                                conn,
                                window,
                                atoms.netWmStateAbove);
                    }
 
                    return true;
                }
 
                if (responseType != XCB_CLIENT_MESSAGE)
                    return false;
 
                const auto* clientMessage =
                    reinterpret_cast<
                        const xcb_client_message_event_t*>(
                        event);
 
                if (clientMessage->window != window)
                    return false;
 
                if (clientMessage->type != protocolsAtom)
                    return false;
 
                if (clientMessage->data.data32[0] != deleteAtom)
                    return false;
 
                closeRequested = true;
 
                return true;
            }
 
            // НЕ блокирующий.
            // true  -> событие получено (и обработано / проигнорировано)
            // false -> очередь для этого окна пуста
            //
            // Использование:  while (window.PollEvent()) {}
            Types::b8 Window::PollEvent()
            {
                xcb_generic_event_t* event =
                    EventQueue::Instance().PollEvent(window);
 
                if (!event)
                    return false;
 
                HandleEvent(event);
 
                std::free(event);
 
                return true;
            }
 
            Types::b8 Window::ShouldClose() const
            {
                return closeRequested;
            }
 
            void Window::SetTitle(
                const Types::String& title)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                xcb_change_property(
                    conn,
                    XCB_PROP_MODE_REPLACE,
                    window,
                    XCB_ATOM_WM_NAME,
                    XCB_ATOM_STRING,
                    8,
                    title.size(),
                    title.c_str());
 
                if (netWmNameAtom && utf8StringAtom)
                {
                    xcb_change_property(
                        conn,
                        XCB_PROP_MODE_REPLACE,
                        window,
                        netWmNameAtom,
                        utf8StringAtom,
                        8,
                        title.size(),
                        title.c_str());
                }
 
                xcb_flush(conn);
            }
 
            std::pair<Types::i32, Types::i32>
            Window::GetPosition() const
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                xcb_get_geometry_cookie_t cookie =
                    xcb_get_geometry(
                        conn,
                        window);
 
                xcb_get_geometry_reply_t* reply =
                    xcb_get_geometry_reply(
                        conn,
                        cookie,
                        nullptr);
 
                if (!reply)
                    return {0, 0};
 
                xcb_translate_coordinates_cookie_t translateCookie =
                    xcb_translate_coordinates(
                        conn,
                        window,
                        reply->root,
                        0,
                        0);
 
                xcb_translate_coordinates_reply_t* translateReply =
                    xcb_translate_coordinates_reply(
                        conn,
                        translateCookie,
                        nullptr);
 
                std::pair<Types::i32, Types::i32> result{0, 0};
 
                if (translateReply)
                {
                    result = {
                        translateReply->dst_x,
                        translateReply->dst_y
                    };
 
                    std::free(translateReply);
                }
 
                std::free(reply);
 
                return result;
            }
 
            void Window::SetPosition(
                Types::i32 x,
                Types::i32 y)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Types::u32 values[] = {
                    static_cast<Types::u32>(x),
                    static_cast<Types::u32>(y)
                };
 
                xcb_configure_window(
                    conn,
                    window,
                    XCB_CONFIG_WINDOW_X |
                        XCB_CONFIG_WINDOW_Y,
                    values);
 
                xcb_flush(conn);
            }
 
            std::pair<Types::i32, Types::i32>
            Window::GetSize() const
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                xcb_get_geometry_cookie_t cookie =
                    xcb_get_geometry(
                        conn,
                        window);
 
                xcb_get_geometry_reply_t* reply =
                    xcb_get_geometry_reply(
                        conn,
                        cookie,
                        nullptr);
 
                if (!reply)
                    return {0, 0};
 
                std::pair<Types::i32, Types::i32> result{
                    reply->width,
                    reply->height
                };
 
                std::free(reply);
 
                return result;
            }
 
            void Window::SetSize(
                Types::i32 width,
                Types::i32 height)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Types::u32 values[] = {
                    static_cast<Types::u32>(width),
                    static_cast<Types::u32>(height)
                };
 
                xcb_configure_window(
                    conn,
                    window,
                    XCB_CONFIG_WINDOW_WIDTH |
                        XCB_CONFIG_WINDOW_HEIGHT,
                    values);
 
                xcb_flush(conn);
            }
 
            void Window::SetResizable(
                Types::b8 resizable)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                isResizable = resizable;
 
                // Раскладка WM_SIZE_HINTS (18 x CARD32)
                struct
                {
                    Types::u32 flags;
                    Types::i32 x, y;
                    Types::i32 width, height;
                    Types::i32 min_width, min_height;
                    Types::i32 max_width, max_height;
                    Types::i32 width_inc, height_inc;
                    Types::i32 min_aspect_num, min_aspect_den;
                    Types::i32 max_aspect_num, max_aspect_den;
                    Types::i32 base_width, base_height;
                    Types::u32 win_gravity;
                } hints = {};
 
                constexpr Types::u32 PMinSize = 1 << 4;
                constexpr Types::u32 PMaxSize = 1 << 5;
 
                hints.flags =
                    PMinSize |
                    PMaxSize;
 
                if (!resizable)
                {
                    auto [width, height] = GetSize();
 
                    hints.min_width = width;
                    hints.min_height = height;
                    hints.max_width = width;
                    hints.max_height = height;
                }
                else
                {
                    hints.min_width = 1;
                    hints.min_height = 1;
                    hints.max_width = 32767;
                    hints.max_height = 32767;
                }
 
                xcb_change_property(
                    conn,
                    XCB_PROP_MODE_REPLACE,
                    window,
                    XCB_ATOM_WM_NORMAL_HINTS,
                    XCB_ATOM_WM_SIZE_HINTS,
                    32,
                    sizeof(hints) / 4,
                    &hints);
 
                xcb_flush(conn);
            }
 
            Types::b8 Window::IsResizable() const
            {
                return isResizable;
            }
 
            void Window::Show()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                xcb_map_window(
                    conn,
                    window);
 
                xcb_flush(conn);
            }
 
            void Window::Hide()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                xcb_unmap_window(
                    conn,
                    window);
 
                xcb_flush(conn);
            }
 
            void Window::Minimize()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                if (!atoms.wmChangeState)
                    return;
 
                // 3 = IconicState
                SendRootClientMessage(
                    conn,
                    window,
                    atoms.wmChangeState,
                    3);
            }
 
            void Window::Maximize()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                if (atoms.netWmStateMaxVert && atoms.netWmStateMaxHorz)
                {
                    SendNetWmStateMessage(
                        conn,
                        window,
                        1,
                        atoms.netWmStateMaxVert,
                        atoms.netWmStateMaxHorz);
                }
            }
 
            void Window::Restore()
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                if (atoms.netWmStateMaxVert && atoms.netWmStateMaxHorz)
                {
                    SendNetWmStateMessage(
                        conn,
                        window,
                        0,
                        atoms.netWmStateMaxVert,
                        atoms.netWmStateMaxHorz);
                }
            }
 
            void Window::SetFullscreen(
                Types::b8 fullscreen)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                if (!atoms.netWmStateFullscreen)
                    return;
 
                SendNetWmStateMessage(
                    conn,
                    window,
                    fullscreen ? 1 : 0,
                    atoms.netWmStateFullscreen);
 
                isFullscreen = fullscreen;
            }
 
            Types::b8 Window::IsFullscreen() const
            {
                return isFullscreen;
            }
 
            void Window::SetAlwaysOnTop(
                Types::b8 onTop)
            {
                xcb_connection_t* conn =
                    Connection::Instance().get();
 
                const Atoms& atoms =
                    GetAtoms(conn);
 
                if (!atoms.netWmStateAbove)
                    return;
 
                SendNetWmStateMessage(
                    conn,
                    window,
                    onTop ? 1 : 0,
                    atoms.netWmStateAbove);
 
                isAlwaysOnTop = onTop;
            }
 
            Types::b8 Window::IsAlwaysOnTop() const
            {
                return isAlwaysOnTop;
            }
 
            Types::b8 Window::WasResized()
            {
                Types::b8 result =
                    resizedFlag;
 
                resizedFlag = false;
 
                return result;
            }
 
            Types::b8 Window::WasMoved()
            {
                Types::b8 result =
                    movedFlag;
 
                movedFlag = false;
 
                return result;
            }
 
            Types::b8 Window::HasFocus() const
            {
                return hasFocus;
            }
        }
    }
}