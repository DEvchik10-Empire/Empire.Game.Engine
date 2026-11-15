#pragma once

#include <utility>

#include "Kernel/Types.hpp"
#include "Kernel/NativeWindowHandle.hpp"
#include "WindowConfig.hpp"

namespace Engine
{
    namespace Platform
    {
        class IWindowImpl
        {
        public:
            virtual ~IWindowImpl() = default;

            IWindowImpl(const IWindowImpl&) = delete;
            IWindowImpl& operator=(const IWindowImpl&) = delete;

            virtual Types::b8 PollEvent() = 0;
            virtual Types::b8 ShouldClose() const = 0;
            virtual void Close() = 0;
            virtual Types::b8 IsOpen() const = 0;

            virtual void SetTitle(const Types::String& title) = 0;

            virtual std::pair<Types::i32, Types::i32>
            GetPosition() const = 0;

            virtual void SetPosition(
                Types::i32 x,
                Types::i32 y
            ) = 0;

            virtual std::pair<Types::i32, Types::i32>
            GetSize() const = 0;

            virtual void SetSize(
                Types::i32 width,
                Types::i32 height
            ) = 0;

            virtual void SetResizable(
                Types::b8 resizable
            ) = 0;

            virtual Types::b8 IsResizable() const = 0;

            virtual void Show() = 0;
            virtual void Hide() = 0;
            virtual void Minimize() = 0;
            virtual void Maximize() = 0;
            virtual void Restore() = 0;

            virtual void SetFullscreen(
                Types::b8 fullscreen
            ) = 0;

            virtual Types::b8 IsFullscreen() const = 0;

            virtual void SetAlwaysOnTop(
                Types::b8 onTop
            ) = 0;

            virtual Types::b8 IsAlwaysOnTop() const = 0;

            virtual Types::b8 WasResized() = 0;
            virtual Types::b8 WasMoved() = 0;
            virtual Types::b8 HasFocus() const = 0;

            virtual WindowBackend GetBackend() const = 0;

            // Нативные хэндлы окна для рендера (см. Kernel/NativeWindowHandle.hpp).
            // Если окно закрыто, kind == None.
            virtual NativeWindowHandle GetNativeHandle() const = 0;

        protected:
            IWindowImpl() = default;
        };
    }
}