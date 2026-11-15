target("Platform.Process")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Process.Windows") end
if is_plat("linux") then includes("Process.Linux") end
if is_plat("macosx") then includes("Process.MacOS") end
