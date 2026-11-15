target("Platform.App")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

-- бэкенды: Noop собирается везде, остальные выбираются платформой
includes("App.Noop")
if is_plat("android") then includes("App.Android") end
if is_plat("macosx") then includes("App.Cocoa") end
if is_plat("iphoneos") then includes("App.UIKit") end
if is_plat("wasm") then includes("App.WASM") end
if is_plat("windows") then includes("App.Win32") end
if is_plat("windows") and has_config("uwp") then includes("App.UWP") end
if is_plat("linux") then includes("App.Wayland") end
if is_plat("linux") then includes("App.XCB") end
if is_plat("linux") then includes("App.XLib") end
