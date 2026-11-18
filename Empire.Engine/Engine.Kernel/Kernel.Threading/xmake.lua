target("Kernel.Threading")
    -- файлы добавляют бэкенды, см. includes ниже

if is_plat("linux") then includes("Threading.Linux") end
if is_plat("macosx") then includes("Threading.MacOS") end
if is_plat("windows") then includes("Threading.Windows") end
