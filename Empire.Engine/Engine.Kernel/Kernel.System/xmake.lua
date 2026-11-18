target("Kernel.System")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("android") then includes("System.Android") end
if is_plat("iphoneos") then includes("System.IOS") end
if is_plat("linux") then includes("System.Linux") end
if is_plat("macosx") then includes("System.MacOS") end
if is_plat("windows") and has_config("uwp") then includes("System.UWP") end
if is_plat("wasm") then includes("System.WASM") end
if is_plat("windows") and not has_config("uwp") then includes("System.Windows") end
