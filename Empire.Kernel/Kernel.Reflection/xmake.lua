target("Kernel.Reflection")
    add_headerfiles("*.hpp")
    add_files("Reflection.TypeRegistry.cpp")
    add_deps("Kernel.Types", "Kernel.Containers")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})

    if is_plat("linux") then
        add_syslinks("pthread")
    end
