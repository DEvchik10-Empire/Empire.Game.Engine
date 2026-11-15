target("Engine.Input")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Platform.Input", "Kernel.Events", "Kernel.Containers")

includes(
    "Input.Action",
    "Input.Context",
    "Input.Binding"
)
