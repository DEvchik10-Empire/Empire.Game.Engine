target("Math.Quaternion")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector", "Math.Matrix")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
