target("Platform.ThirdParty")
    set_kind("headeronly")
    add_includedirs(".", {public = true})

if is_plat("linux") then
    target("Platform.ThirdParty.Wayland")
        set_kind("static")
        set_warnings("none")
        add_files("ThirdParty.Wayland/Wayland.XdgShell.cpp")
        add_includedirs(".", {public = true})
end
