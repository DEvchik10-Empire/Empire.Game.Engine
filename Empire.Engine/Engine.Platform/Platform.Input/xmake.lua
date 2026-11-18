target("Platform.Input")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Input.Noop")
if is_plat("android") then includes("Input.Android") end
if is_plat("macosx") then includes("Input.Cocoa") end
if is_plat("iphoneos") then includes("Input.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Input.UWP") end
if is_plat("wasm") then includes("Input.WASM") end
if is_plat("linux") then includes("Input.Wayland") end
if is_plat("windows") then includes("Input.Win32") end
if is_plat("linux") then includes("Input.Xcb") end
if is_plat("linux") then includes("Input.XLib") end
