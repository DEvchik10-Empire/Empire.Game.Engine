#pragma once

#include <utility>

#include <xcb/xcb.h>

#include "Kernel/Types.hpp"
#include "../WindowConfig.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            using namespace Kernel;

            class Window
            {
            public:
                Window(const WindowConfig& config);
                ~Window();

                xcb_window_t Get() const;

                Types::b8 PollEvent();
                Types::b8 ShouldClose() const;

                void SetTitle(const Types::String& title);

                std::pair<Types::i32, Types::i32> GetPosition() const;
                void SetPosition(Types::i32 x, Types::i32 y);

                std::pair<Types::i32, Types::i32> GetSize() const;
                void SetSize(Types::i32 width, Types::i32 height);

                void SetResizable(Types::b8 resizable);
                Types::b8 IsResizable() const;

                void Show();
                void Hide();
                void Minimize();
                void Maximize();
                void Restore();

                void SetFullscreen(Types::b8 fullscreen);
                Types::b8 IsFullscreen() const;
                void SetAlwaysOnTop(Types::b8 onTop);
                Types::b8 IsAlwaysOnTop() const;

                Types::b8 WasResized();
                Types::b8 WasMoved();
                Types::b8 HasFocus() const;

            private:
                Types::b8 HandleEvent(const xcb_generic_event_t* event);

                xcb_window_t window = 0;

                xcb_atom_t protocolsAtom = 0;
                xcb_atom_t deleteAtom = 0;
                xcb_atom_t netWmNameAtom = 0;
                xcb_atom_t utf8StringAtom = 0;

                Types::b8 closeRequested = false;
                Types::b8 isResizable = true;
                Types::b8 isFullscreen = false;
                Types::b8 isAlwaysOnTop = false;

                Types::b8 resizedFlag = false;
                Types::b8 movedFlag = false;
                Types::b8 hasFocus = true;

                Types::i32 lastKnownWidth = 0;
                Types::i32 lastKnownHeight = 0;
                Types::i32 lastKnownX = 0;
                Types::i32 lastKnownY = 0;
            };
        }
    }
}