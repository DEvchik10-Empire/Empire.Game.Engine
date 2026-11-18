target("Physics.Shape")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Shape.2D",
    "Shape.3D"
)
