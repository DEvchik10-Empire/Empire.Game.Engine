target("Platform.Dialog")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")

if is_plat("windows") then includes("Dialog.Win32") end
if is_plat("linux") then includes("Dialog.Wayland") end
if is_plat("linux") then includes("Dialog.Legacy") end
if is_plat("macosx") then includes("Dialog.Cocoa") end
