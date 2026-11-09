target("Platform.Clipboard")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App")

-- бэкенды: Noop собирается везде, остальные выбираются платформой
includes("Clipboard.Noop")
if is_plat("android") then includes("Clipboard.Android") end
if is_plat("macosx") then includes("Clipboard.Cocoa") end
if is_plat("iphoneos") then includes("Clipboard.UIKit") end
if is_plat("wasm") then includes("Clipboard.WASM") end
if is_plat("windows") then includes("Clipboard.Win32") end
if is_plat("windows") and has_config("uwp") then includes("Clipboard.UWP") end
if is_plat("linux") then includes("Clipboard.Wayland") end
if is_plat("linux") then includes("Clipboard.XCB") end
if is_plat("linux") then includes("Clipboard.XLib") end
