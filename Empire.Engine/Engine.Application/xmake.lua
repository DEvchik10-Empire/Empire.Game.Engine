target("Engine.Application")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.Modules", "Engine.GameLoop", "Platform.App", "Kernel.Services", "Kernel.Config", "Kernel.Logging")

includes(
    "Application.Entry",
    "Application.Lifecycle",
    "Application.CommandLine"
)
