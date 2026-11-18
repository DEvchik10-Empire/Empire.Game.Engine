target("Kernel.Library")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Library.Linux") end
if is_plat("macosx") then includes("Library.MacOS") end
if is_plat("windows") then includes("Library.Windows") end
