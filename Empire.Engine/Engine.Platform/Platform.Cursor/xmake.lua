target("Platform.Cursor")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Cursor.Noop")
if is_plat("android") then includes("Cursor.Android") end
if is_plat("macosx") then includes("Cursor.Cocoa") end
if is_plat("iphoneos") then includes("Cursor.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Cursor.UWP") end
if is_plat("wasm") then includes("Cursor.WASM") end
if is_plat("linux") then includes("Cursor.Wayland") end
if is_plat("windows") then includes("Cursor.Win32") end
if is_plat("linux") then includes("Cursor.Xcb") end
if is_plat("linux") then includes("Cursor.XLib") end
