target("Math.Geometry")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector")

includes(
    "Geometry.2D",
    "Geometry.3D"
)
