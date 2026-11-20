target("Math.Matrix")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Compiler", "Kernel.Containers", "Math.Vector", "Math.Scalar")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
