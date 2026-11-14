target("Engine.Physics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Constraint.2D",
    "Constraint.3D"
)
