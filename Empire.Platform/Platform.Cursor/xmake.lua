target("Platform.Cursor")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

-- бэкенды: Noop собирается везде, остальные выбираются платформой
includes("Cursor.Noop")
if is_plat("android") then includes("Cursor.Android") end
if is_plat("macosx") then includes("Cursor.Cocoa") end
if is_plat("iphoneos") then includes("Cursor.UIKit") end
if is_plat("wasm") then includes("Cursor.WASM") end
if is_plat("windows") then includes("Cursor.Win32") end
if is_plat("windows") and has_config("uwp") then includes("Cursor.UWP") end
if is_plat("linux") then includes("Cursor.Wayland") end
if is_plat("linux") then includes("Cursor.XCB") end
if is_plat("linux") then includes("Cursor.XLib") end
