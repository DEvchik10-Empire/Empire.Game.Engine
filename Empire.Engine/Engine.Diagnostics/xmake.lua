target("Engine.Diagnostics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.Process", "Platform.Time", "Kernel.Logging")
