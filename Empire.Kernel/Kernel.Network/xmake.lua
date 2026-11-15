target("Kernel.Network")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})

if is_plat("windows") then includes("Network.Windows") end
if is_plat("linux") then includes("Network.Linux") end
if is_plat("macosx") then includes("Network.MacOS") end
