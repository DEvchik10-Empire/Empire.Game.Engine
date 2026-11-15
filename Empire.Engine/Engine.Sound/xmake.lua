target("Engine.Sound")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Engine.Assets", "Platform.Audio", "Math.Vector")

includes(
    "Sound.Dsp",
    "Sound.Codec",
    "Sound.Stream",
    "Sound.Effect",
    "Sound.Mixer",
    "Sound.Spatial",
    "Sound.Source",
    "Sound.Listener",
    "Sound.Bank",
    "Sound.Music"
)
