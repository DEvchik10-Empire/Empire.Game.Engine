target("Platform.Events")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Events.Noop")
if is_plat("android") then includes("Events.Android") end
if is_plat("macosx") then includes("Events.Cocoa") end
if is_plat("iphoneos") then includes("Events.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Events.UWP") end
if is_plat("wasm") then includes("Events.WASM") end
if is_plat("linux") then includes("Events.Wayland") end
if is_plat("windows") then includes("Events.Win32") end
if is_plat("linux") then includes("Events.Xcb") end
if is_plat("linux") then includes("Events.XLib") end
