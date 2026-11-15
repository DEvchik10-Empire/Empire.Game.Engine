target("Engine.Physics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Solver.2D",
    "Solver.3D"
)
