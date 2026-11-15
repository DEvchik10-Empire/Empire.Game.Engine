set_project("Empire.Platform")

set_languages("c++26")

add_rules("mode.debug", "mode.release")

local EngineRoot = path.absolute("../..")
local PlatformRoot = path.absolute(".")

add_requires(
    "pkgconfig::wayland-client",
    "pkgconfig::xcb"
)


target("Empire.Platform")
    set_kind("static")

    add_files(
    "Empire.App/Wayland/*.cpp",
    "Empire.App/legasy/*.cpp",
    "Empire.Window/*.cpp",
    "Empire.Window/Wayland/*.cpp",
    "Empire.Window/legasy/*.cpp",
    "ThirdParty/Wayland/xdg-shell-protocol.c"
    )

    add_includedirs(
        EngineRoot,
        PlatformRoot,
        path.join(PlatformRoot, "ThirdParty/Wayland")
    )

    add_packages(
        "pkgconfig::wayland-client",
        "pkgconfig::xcb"
    )


target("Empire.Platform.Test.Wayland")
    set_kind("binary")

    add_files(
        "Tests/Wayland/TestMain.cpp"
    )

    add_deps("Empire.Platform")

    add_includedirs(
        EngineRoot,
        PlatformRoot,
        path.join(PlatformRoot, "ThirdParty/Wayland")
    )

    add_packages(
        "pkgconfig::wayland-client"
    )


target("Empire.Platform.Test.legasy")
    set_kind("binary")

    add_files(
        "Tests/legasy/TestMain.cpp"
    )

    add_deps("Empire.Platform")

    add_includedirs(
        EngineRoot,
        PlatformRoot
    )

    add_packages(
        "pkgconfig::xcb"
    )

target("Empire.Platform.Test.auto")
    set_kind("binary")

    add_files(
        "Tests/test.cpp"
    )

    add_deps("Empire.Platform")

    add_includedirs(
        EngineRoot,
        PlatformRoot,
        path.join(PlatformRoot, "ThirdParty/Wayland")
    )

    add_packages(
        "pkgconfig::xcb",
        "pkgconfig::wayland-client"
    )