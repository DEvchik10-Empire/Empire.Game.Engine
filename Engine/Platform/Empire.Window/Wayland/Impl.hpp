#pragma once

#include <utility>

#include "WindowConfig.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class Impl
            {
            public:
                explicit Impl(
                    const WindowConfig& config
                );

                ~Impl();

                Impl(const Impl&) = delete;
                Impl& operator=(const Impl&) = delete;

                Types::b8 PollEvent();

                Types::b8 ShouldClose() const;

                void Close();

                Types::b8 IsOpen() const;

                void SetTitle(
                    const char* title
                );

                std::pair<Types::i32, Types::i32>
                GetPosition() const;

                void SetPosition(
                    Types::i32 x,
                    Types::i32 y
                );

                std::pair<Types::i32, Types::i32>
                GetSize() const;

                void SetSize(
                    Types::i32 width,
                    Types::i32 height
                );

                void SetResizable(
                    Types::b8 resizable
                );

                Types::b8 IsResizable() const;

                void Show();
                void Hide();
                void Minimize();
                void Maximize();
                void Restore();

                void SetFullscreen(
                    Types::b8 fullscreen
                );

                Types::b8 IsFullscreen() const;

                void SetAlwaysOnTop(
                    Types::b8 onTop
                );

                Types::b8 IsAlwaysOnTop() const;

                Types::b8 WasResized();
                Types::b8 WasMoved();

                Types::b8 HasFocus() const;

            private:
                struct Data;

                Data* data = nullptr;
            };
        }
    }
}