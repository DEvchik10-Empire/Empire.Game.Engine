target("Engine.Application")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.Modules", "Engine.GameLoop", "Platform.App", "Kernel.Services", "Kernel.Logging")

includes(
    "Application.Entry",
    "Application.Lifecycle",
    "Application.CommandLine"
)
v1.1.0-Dev-26.11.12