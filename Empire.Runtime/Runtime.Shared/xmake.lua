target("Runtime.Shared")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.Application", "Engine.ECS", "Engine.Node", "Engine.Gameplay", "Engine.Networking", "Engine.Assets", "Engine.Physics", "Engine.AI")
