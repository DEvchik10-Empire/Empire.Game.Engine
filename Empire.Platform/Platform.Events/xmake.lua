target("Platform.Events")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Events.Win32") end
if is_plat("linux") then includes("Events.Wayland") end
if is_plat("linux") then includes("Events.Legacy") end
if is_plat("macosx") then includes("Events.Cocoa") end
