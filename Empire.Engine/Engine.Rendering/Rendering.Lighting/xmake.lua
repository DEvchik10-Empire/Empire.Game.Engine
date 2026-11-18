target("Rendering.Lighting")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Lighting.2D",
    "Lighting.3D"
)
