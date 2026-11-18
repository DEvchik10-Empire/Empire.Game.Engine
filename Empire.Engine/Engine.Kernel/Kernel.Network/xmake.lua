target("Kernel.Network")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Network.Linux") end
if is_plat("macosx") then includes("Network.MacOS") end
if is_plat("windows") then includes("Network.Windows") end
