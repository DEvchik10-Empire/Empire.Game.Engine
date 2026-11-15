set_project("Empire.Rendering")

set_languages("c++26")

add_rules("mode.debug", "mode.release")

local EngineRoot = path.absolute("../..")
local RenderingRoot = path.absolute(".")

add_requires(
    "pkgconfig::vulkan",
    "pkgconfig::wayland-client",
    "pkgconfig::xcb"
)


target("Empire.Rendering")
    set_kind("static")

    add_files(
        "Empire.Device/Vulkan/*.cpp",
        "Empire.SwapChain/Vulkan/*.cpp",
        "Empire.CommandBuffer/Vulkan/*.cpp",
        "Empire.Buffer/Vulkan/*.cpp",
        "Empire.Texture/*.cpp",
        "Empire.Texture/Vulkan/*.cpp",
        "Empire.Shader/Vulkan/*.cpp",
        "Empire.Pipeline/Vulkan/*.cpp",
        "Empire.Mesh/*.cpp"
    )

    add_includedirs(
        EngineRoot,
        RenderingRoot
    )

    -- wayland-client и xcb нужны только ради заголовков:
    -- vulkan_wayland.h / vulkan_xcb.h (VK_USE_PLATFORM_*).
    add_packages(
        "pkgconfig::vulkan",
        "pkgconfig::wayland-client",
        "pkgconfig::xcb"
    )
