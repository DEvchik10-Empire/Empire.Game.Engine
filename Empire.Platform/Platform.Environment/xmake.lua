target("Platform.Environment")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Environment.Windows") end
if is_plat("linux") then includes("Environment.Linux") end
if is_plat("macosx") then includes("Environment.MacOS") end
