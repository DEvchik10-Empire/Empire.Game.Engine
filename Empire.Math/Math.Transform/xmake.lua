target("Math.Transform")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Math.Vector", "Math.Matrix", "Math.Quaternion")

includes(
    "Transform.2D",
    "Transform.3D",
    "Transform.World"
)
