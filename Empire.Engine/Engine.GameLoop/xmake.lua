target("Engine.GameLoop")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.Time", "Kernel.Events", "Kernel.Containers")

includes(
    "GameLoop.Clock",
    "GameLoop.FixedStep",
    "GameLoop.Scheduler"
)
