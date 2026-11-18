target("Kernel.FileSystem")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("FileSystem.Linux") end
if is_plat("macosx") then includes("FileSystem.MacOS") end
if is_plat("windows") then includes("FileSystem.Windows") end
