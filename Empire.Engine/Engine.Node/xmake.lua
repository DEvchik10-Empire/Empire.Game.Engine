target("Engine.Node")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Math.Transform")

includes(
    "Node.Hierarchy",
    "Node.Scene",
    "Node.Prefab",
    "Node.Serialization"
)
