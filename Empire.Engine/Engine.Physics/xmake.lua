target("Engine.Physics")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Math.Geometry")

includes(
    "Physics.Shape",
    "Physics.Collision",
    "Physics.Solver",
    "Physics.Constraint",
    "Physics.Body",
    "Physics.Query",
    "Physics.Character",
    "Physics.Debug"
)
