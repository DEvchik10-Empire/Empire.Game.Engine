target("Engine.Physics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Collision.2D",
    "Collision.3D"
)
