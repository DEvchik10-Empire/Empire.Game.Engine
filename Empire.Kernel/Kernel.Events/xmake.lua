target("Kernel.Events")
    add_headerfiles("*.hpp")
    add_files("Events.EventBus.cpp")
    add_deps("Kernel.Types")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})

    if is_plat("linux") then
        add_syslinks("pthread")
    end
