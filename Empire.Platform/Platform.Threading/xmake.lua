target("Platform.Threading")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Threading.Windows") end
if is_plat("linux") then includes("Threading.Linux") end
if is_plat("macosx") then includes("Threading.MacOS") end
