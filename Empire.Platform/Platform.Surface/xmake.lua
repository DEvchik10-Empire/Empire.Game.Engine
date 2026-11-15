target("Platform.Surface")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

if is_plat("windows") then includes("Surface.Win32") end
if is_plat("linux") then includes("Surface.Wayland") end
if is_plat("linux") then includes("Surface.Legacy") end
if is_plat("macosx") then includes("Surface.Cocoa") end
