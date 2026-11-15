set_project("Empire.Tests")

set_languages("c++26")

add_rules("mode.debug", "mode.release")

local EngineRoot = path.absolute("..")
local PlatformRoot = path.join(EngineRoot, "Engine/Platform")
local RenderingRoot = path.join(EngineRoot, "Engine/Rendering")

add_requires(
    "pkgconfig::vulkan",
    "pkgconfig::wayland-client",
    "pkgconfig::xcb"
)


-- Тест рендеринга: окно (Platform) + Vulkan (Rendering) + закраска цветом.
--
--   xmake build Empire.Rendering.Test.Vulkan
--   xmake run   Empire.Rendering.Test.Vulkan             (Auto)
--   xmake run   Empire.Rendering.Test.Vulkan wayland
--   xmake run   Empire.Rendering.Test.Vulkan legasy
--   xmake run   Empire.Rendering.Test.Vulkan --frames=300
target("Empire.Rendering.Test.Vulkan")
    set_kind("binary")

    add_files(
        "Rendering/TestMain.cpp"
    )

    add_deps(
        "Empire.Platform",
        "Empire.Rendering"
    )

    add_includedirs(
        EngineRoot,
        PlatformRoot,
        RenderingRoot,
        path.join(PlatformRoot, "ThirdParty/Wayland")
    )

    add_packages(
        "pkgconfig::vulkan",
        "pkgconfig::wayland-client",
        "pkgconfig::xcb"
    )


-- Тест 3D-сцены: небо (HLSL) + вращающаяся модель.
-- Шейдеры собираются отдельно (нужен dxc):
--
--   xmake build Empire.Shaders
--   xmake build Empire.Rendering.Test.Scene
--   xmake run   Empire.Rendering.Test.Scene --time=night
--   xmake run   Empire.Rendering.Test.Scene --cycle --mesh=sphere
--   xmake run   Empire.Rendering.Test.Scene --model=path/to/model.glb
--   xmake run   Empire.Rendering.Test.Scene --show=cube|model|none
target("Empire.Rendering.Test.Scene")
    set_kind("binary")

    add_files(
        "Rendering/SceneMain.cpp"
    )

    add_deps(
        "Empire.Platform",
        "Empire.Rendering",
        "Empire.Shaders"
    )

    add_includedirs(
        EngineRoot,
        PlatformRoot,
        RenderingRoot,
        path.join(PlatformRoot, "ThirdParty/Wayland")
    )

    add_defines('EMPIRE_SHADER_DIR="' .. path.join(EngineRoot, "build", "shaders") .. '"')

    -- Папка с моделями: Assets/asset.glb показывается по умолчанию.
    add_defines('EMPIRE_ASSET_DIR="' .. path.join(EngineRoot, "Assets") .. '"')

    add_packages(
        "pkgconfig::vulkan",
        "pkgconfig::wayland-client",
        "pkgconfig::xcb"
    )
