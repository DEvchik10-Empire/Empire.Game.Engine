target("Engine.Gameplay")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Kernel.Events")

includes(
    "Gameplay.Tag",
    "Gameplay.Attribute",
    "Gameplay.Effect",
    "Gameplay.Ability",
    "Gameplay.Cue"
)
