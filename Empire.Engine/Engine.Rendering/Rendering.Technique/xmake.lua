target("Engine.Rendering")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Technique.2D",
    "Technique.3D"
)
