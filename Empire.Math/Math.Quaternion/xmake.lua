target("Math.Quaternion")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector", "Math.Matrix")
