target("Platform.Input")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Platform.App", "Platform.Window")

-- бэкенды: Noop собирается везде, остальные выбираются платформой
includes("Input.Noop")
if is_plat("android") then includes("Input.Android") end
if is_plat("macosx") then includes("Input.Cocoa") end
if is_plat("iphoneos") then includes("Input.UIKit") end
if is_plat("wasm") then includes("Input.WASM") end
if is_plat("windows") then includes("Input.Win32") end
if is_plat("windows") and has_config("uwp") then includes("Input.UWP") end
if is_plat("linux") then includes("Input.Wayland") end
if is_plat("linux") then includes("Input.XCB") end
if is_plat("linux") then includes("Input.XLib") end
