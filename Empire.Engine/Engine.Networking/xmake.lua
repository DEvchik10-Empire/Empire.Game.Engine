target("Engine.Networking")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Jobs", "Platform.Network", "Kernel.Events")

includes(
    "Networking.Bitstream",
    "Networking.Transport",
    "Networking.Session",
    "Networking.Message",
    "Networking.Snapshot",
    "Networking.Replication",
    "Networking.Prediction",
    "Networking.Lobby",
    "Networking.Debug"
)
