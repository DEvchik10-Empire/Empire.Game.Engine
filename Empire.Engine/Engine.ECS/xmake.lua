target("Engine.ECS")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Containers", "Kernel.Memory", "Kernel.Reflection", "Kernel.Events")

includes(
    "ECS.Entity",
    "ECS.Component",
    "ECS.Archetype",
    "ECS.Query",
    "ECS.System",
    "ECS.Schedule"
)
