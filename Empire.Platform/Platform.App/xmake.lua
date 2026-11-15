target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("App.Win32") end
if is_plat("linux") then includes("App.Wayland") end
if is_plat("linux") then includes("App.Legacy") end
if is_plat("macosx") then includes("App.Cocoa") end
