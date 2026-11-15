target("Platform.Monitor")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")

if is_plat("windows") then includes("Monitor.Win32") end
if is_plat("linux") then includes("Monitor.Wayland") end
if is_plat("linux") then includes("Monitor.Legacy") end
if is_plat("macosx") then includes("Monitor.Cocoa") end
