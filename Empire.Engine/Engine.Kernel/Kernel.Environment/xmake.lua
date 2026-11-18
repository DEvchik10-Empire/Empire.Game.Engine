target("Kernel.Environment")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Environment.Linux") end
if is_plat("macosx") then includes("Environment.MacOS") end
if is_plat("windows") then includes("Environment.Windows") end
