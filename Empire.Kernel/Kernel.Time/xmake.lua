target("Kernel.Time")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})

if is_plat("windows") then includes("Time.Windows") end
if is_plat("linux") then includes("Time.Linux") end
if is_plat("macosx") then includes("Time.MacOS") end
