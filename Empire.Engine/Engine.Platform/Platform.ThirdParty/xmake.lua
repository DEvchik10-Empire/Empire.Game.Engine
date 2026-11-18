target("Platform.ThirdParty")
    set_kind("headeronly")
    add_headerfiles("*/*.hpp")

if is_plat("linux") then
    target("Platform.ThirdParty.Wayland")
        set_kind("static")
        set_warnings("none")
        add_files("ThirdParty.Wayland/Wayland.XdgShell.cpp")
end
