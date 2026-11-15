target("Platform.Clipboard")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")

if is_plat("windows") then includes("Clipboard.Win32") end
if is_plat("linux") then includes("Clipboard.Wayland") end
if is_plat("linux") then includes("Clipboard.Legacy") end
if is_plat("macosx") then includes("Clipboard.Cocoa") end
