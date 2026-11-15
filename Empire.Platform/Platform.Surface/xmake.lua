target("Platform.Surface")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

-- бэкенды: Noop собирается везде, остальные выбираются платформой
includes("Surface.Noop")
if is_plat("android") then includes("Surface.Android") end
if is_plat("macosx") then includes("Surface.Cocoa") end
if is_plat("iphoneos") then includes("Surface.UIKit") end
if is_plat("wasm") then includes("Surface.WASM") end
if is_plat("windows") then includes("Surface.Win32") end
if is_plat("windows") and has_config("uwp") then includes("Surface.UWP") end
if is_plat("linux") then includes("Surface.Wayland") end
if is_plat("linux") then includes("Surface.XCB") end
if is_plat("linux") then includes("Surface.XLib") end
