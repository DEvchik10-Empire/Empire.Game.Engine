target("Platform.Library")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")

if is_plat("windows") then includes("Library.Windows") end
if is_plat("linux") then includes("Library.Linux") end
if is_plat("macosx") then includes("Library.MacOS") end
