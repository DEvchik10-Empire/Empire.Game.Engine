target("Kernel.Config")
    set_kind("headeronly")
    add_headerfiles("*.hpp")
    add_deps("Kernel.Compiler", "Kernel.Types")
