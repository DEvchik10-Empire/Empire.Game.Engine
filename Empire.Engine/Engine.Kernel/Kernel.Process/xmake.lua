target("Kernel.Process")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Process.Linux") end
if is_plat("macosx") then includes("Process.MacOS") end
if is_plat("windows") then includes("Process.Windows") end
