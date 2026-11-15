target("Math.Geometry")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector", "Math.Matrix")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})

includes(
    "Geometry.2D",
    "Geometry.3D"
)
