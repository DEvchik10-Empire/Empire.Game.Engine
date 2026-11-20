target("Geometry.3D")
    add_deps("Kernel.Types", "Kernel.Assert", "Kernel.Compiler", "Kernel.Containers", "Kernel.Memory", "Math.Vector", "Math.Matrix", "Geometry.2D")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
