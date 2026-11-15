#pragma once

#include <utility>

#include "Kernel/Types.hpp"
#include "../WindowConfig.hpp"
#include "../IWindowImpl.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            using namespace Kernel;

            class Impl : public IWindowImpl
            {
            public:
                explicit Impl(const WindowConfig& config);
                ~Impl() override;

                Impl(const Impl&) = delete;
                Impl& operator=(const Impl&) = delete;

                Types::b8 PollEvent() override;
                Types::b8 ShouldClose() const override;
                void Close() override;
                Types::b8 IsOpen() const override;

                void SetTitle(
                    const Types::String& title
                ) override;

                std::pair<Types::i32, Types::i32>
                GetPosition() const override;

                void SetPosition(
                    Types::i32 x,
                    Types::i32 y
                ) override;

                std::pair<Types::i32, Types::i32>
                GetSize() const override;

                void SetSize(
                    Types::i32 width,
                    Types::i32 height
                ) override;

                void SetResizable(
                    Types::b8 resizable
                ) override;

                Types::b8 IsResizable() const override;

                void Show() override;
                void Hide() override;
                void Minimize() override;
                void Maximize() override;
                void Restore() override;

                void SetFullscreen(
                    Types::b8 fullscreen
                ) override;

                Types::b8 IsFullscreen() const override;

                void SetAlwaysOnTop(
                    Types::b8 onTop
                ) override;

                Types::b8 IsAlwaysOnTop() const override;

                Types::b8 WasResized() override;
                Types::b8 WasMoved() override;
                Types::b8 HasFocus() const override;

                WindowBackend GetBackend() const override;

                NativeWindowHandle GetNativeHandle() const override;

            private:
                struct Data;

                Data* data = nullptr;
            };
        }
    }
}