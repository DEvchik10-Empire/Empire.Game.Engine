target("Engine.Jobs")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.Threading", "Kernel.Memory", "Kernel.Containers")
