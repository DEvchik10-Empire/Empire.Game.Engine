target("Platform.Cursor")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

if is_plat("windows") then includes("Cursor.Win32") end
if is_plat("linux") then includes("Cursor.Wayland") end
if is_plat("linux") then includes("Cursor.Legacy") end
if is_plat("macosx") then includes("Cursor.Cocoa") end
