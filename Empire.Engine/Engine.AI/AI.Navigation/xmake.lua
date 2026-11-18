target("AI.Navigation")
    add_headerfiles("*.hpp")
    add_files("*.cpp")

includes(
    "Navigation.2D",
    "Navigation.3D"
)
