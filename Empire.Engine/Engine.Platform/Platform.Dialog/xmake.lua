target("Platform.Dialog")
    -- файлы добавляют бэкенды, см. includes ниже

includes("Dialog.Noop")
if is_plat("android") then includes("Dialog.Android") end
if is_plat("macosx") then includes("Dialog.Cocoa") end
if is_plat("iphoneos") then includes("Dialog.UIKit") end
if is_plat("windows") and has_config("uwp") then includes("Dialog.UWP") end
if is_plat("wasm") then includes("Dialog.WASM") end
if is_plat("linux") then includes("Dialog.Wayland") end
if is_plat("windows") then includes("Dialog.Win32") end
if is_plat("linux") then includes("Dialog.Xcb") end
if is_plat("linux") then includes("Dialog.XLib") end
