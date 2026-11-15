target("Engine.AI")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Math.Vector")

includes(
    "AI.Blackboard",
    "AI.Navigation",
    "AI.Steering",
    "AI.BehaviorTree",
    "AI.Utility",
    "AI.Planner",
    "AI.Perception",
    "AI.Agent"
)
