target("Math.Vector")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Compiler", "Kernel.Containers", "Math.Scalar")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
