set_project("Empire.Platform")

set_languages("c++23")

add_rules("mode.debug", "mode.release")

add_requires("pkgconfig::wayland-client")


target("Empire.Platform")
    set_kind("static")

    add_files(
        "Empire.App/Wayland/*.cpp",
        "Empire.Window/Wayland/*.cpp"
    )

    add_files(
        "ThirdParty/Wayland/xdg-shell-protocol.c"
    )

    add_includedirs(
        "../..",
        ".",
        "ThirdParty/Wayland"
    )

    add_packages("pkgconfig::wayland-client")


target("Empire.Platform.Test.Wayland")
    set_kind("binary")

    add_files(
        "Tests/Wayland/TestMain.cpp"
    )

    add_deps("Empire.Platform")

    add_includedirs(
        "../..",
        ".",
        "ThirdParty/Wayland"
    )

    add_packages("pkgconfig::wayland-client")