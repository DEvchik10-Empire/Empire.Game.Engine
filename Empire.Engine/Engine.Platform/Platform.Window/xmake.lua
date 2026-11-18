target("Platform.Window")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Window.Noop")
if is_plat("android") then includes("Window.Android") end
if is_plat("macosx") then includes("Window.Cocoa") end
if is_plat("iphoneos") then includes("Window.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Window.UWP") end
if is_plat("wasm") then includes("Window.WASM") end
if is_plat("linux") then includes("Window.Wayland") end
if is_plat("windows") then includes("Window.Win32") end
if is_plat("linux") then includes("Window.Xcb") end
if is_plat("linux") then includes("Window.XLib") end
