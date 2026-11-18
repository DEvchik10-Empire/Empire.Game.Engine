target("Platform.Monitor")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Monitor.Noop")
if is_plat("android") then includes("Monitor.Android") end
if is_plat("macosx") then includes("Monitor.Cocoa") end
if is_plat("iphoneos") then includes("Monitor.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Monitor.UWP") end
if is_plat("wasm") then includes("Monitor.WASM") end
if is_plat("linux") then includes("Monitor.Wayland") end
if is_plat("windows") then includes("Monitor.Win32") end
if is_plat("linux") then includes("Monitor.Xcb") end
if is_plat("linux") then includes("Monitor.XLib") end
