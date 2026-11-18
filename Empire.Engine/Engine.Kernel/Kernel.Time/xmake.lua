target("Kernel.Time")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Time.Linux") end
if is_plat("macosx") then includes("Time.MacOS") end
if is_plat("windows") then includes("Time.Windows") end
