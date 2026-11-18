target("Math.Vector")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Simd")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
