target("Platform.Input")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

if is_plat("windows") then includes("Input.Win32") end
if is_plat("linux") then includes("Input.Wayland") end
if is_plat("linux") then includes("Input.Legacy") end
if is_plat("macosx") then includes("Input.Cocoa") end
