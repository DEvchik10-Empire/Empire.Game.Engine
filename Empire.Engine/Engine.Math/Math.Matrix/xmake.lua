target("Math.Matrix")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector", "Math.Scalar")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
