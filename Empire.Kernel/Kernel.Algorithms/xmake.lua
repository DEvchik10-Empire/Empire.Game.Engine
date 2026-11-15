target("Kernel.Algorithms")
    set_kind("headeronly")
    add_deps("Kernel.Types", "Kernel.Assert")
    add_headerfiles("*.hpp")
