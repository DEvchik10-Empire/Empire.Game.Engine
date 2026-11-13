target("Kernel.Memory")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.System", "Kernel.Assert", "Kernel.Sync")

