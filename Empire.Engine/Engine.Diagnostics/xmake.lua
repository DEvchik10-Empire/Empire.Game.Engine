target("Engine.Diagnostics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Process", "Kernel.Time", "Kernel.Logging")
