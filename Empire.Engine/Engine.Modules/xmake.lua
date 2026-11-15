target("Engine.Modules")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Services", "Kernel.Containers", "Kernel.Logging")

includes(
    "Modules.Subsystem",
    "Modules.Registry",
    "Modules.Descriptor",
    "Modules.Dependency"
)
