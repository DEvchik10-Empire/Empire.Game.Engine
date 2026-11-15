target("Platform.Window")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")

if is_plat("windows") then includes("Window.Win32") end
if is_plat("linux") then includes("Window.Wayland") end
if is_plat("linux") then includes("Window.Legacy") end
if is_plat("macosx") then includes("Window.Cocoa") end
