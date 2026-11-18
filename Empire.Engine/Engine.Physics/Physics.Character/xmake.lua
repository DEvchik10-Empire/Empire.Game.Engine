target("Physics.Character")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Character.2D",
    "Character.3D"
)
