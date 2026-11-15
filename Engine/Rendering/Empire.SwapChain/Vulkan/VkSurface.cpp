#include "VkSurface.hpp"

#include <stdexcept>

#if defined(VK_USE_PLATFORM_XCB_KHR)
    #include <xcb/xcb.h>
#endif

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            Surface::Surface(const Instance& instance, const NativeWindowHandle& window)
                : instance(instance.Get())
            {
                switch (window.kind)
                {
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
                    case NativeWindowKind::Wayland:
                    {
                        if (!instance.HasExtension(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME))
                        {
                            throw std::runtime_error(
                                "Vulkan: VK_KHR_wayland_surface is not enabled "
                                "in this Instance");
                        }

                        VkWaylandSurfaceCreateInfoKHR info{};
                        info.sType =
                            VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
                        info.display =
                            static_cast<wl_display*>(window.wayland.display);
                        info.surface =
                            static_cast<wl_surface*>(window.wayland.surface);

                        Check(
                            vkCreateWaylandSurfaceKHR(
                                this->instance, &info, nullptr, &handle),
                            "vkCreateWaylandSurfaceKHR");

                        break;
                    }
#endif

#if defined(VK_USE_PLATFORM_XCB_KHR)
                    case NativeWindowKind::Xcb:
                    {
                        if (!instance.HasExtension(VK_KHR_XCB_SURFACE_EXTENSION_NAME))
                        {
                            throw std::runtime_error(
                                "Vulkan: VK_KHR_xcb_surface is not enabled "
                                "in this Instance");
                        }

                        VkXcbSurfaceCreateInfoKHR info{};
                        info.sType =
                            VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
                        info.connection =
                            static_cast<xcb_connection_t*>(window.xcb.connection);
                        info.window =
                            static_cast<xcb_window_t>(window.xcb.window);

                        Check(
                            vkCreateXcbSurfaceKHR(
                                this->instance, &info, nullptr, &handle),
                            "vkCreateXcbSurfaceKHR");

                        break;
                    }
#endif

                    case NativeWindowKind::None:
                    {
                        throw std::runtime_error(
                            "Vulkan: window handle is empty "
                            "(window is closed or was not created)");
                    }

                    default:
                    {
                        throw std::runtime_error(
                            "Vulkan: this window kind is not supported "
                            "on this platform yet");
                    }
                }
            }

            Surface::~Surface()
            {
                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroySurfaceKHR(instance, handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }
            }

            VkSurfaceKHR Surface::Get() const
            {
                return handle;
            }
        }
    }
}
