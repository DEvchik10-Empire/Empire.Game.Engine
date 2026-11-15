target("Kernel.Events")
    add_headerfiles("*.hpp")
    add_files("Events.EventBus.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Memory", "Kernel.Containers", "Kernel.Sync")
