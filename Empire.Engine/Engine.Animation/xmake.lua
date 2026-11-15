target("Engine.Animation")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Engine.Assets", "Math.Transform")

includes(
    "Animation.Curve",
    "Animation.Clip",
    "Animation.Skeleton",
    "Animation.Pose",
    "Animation.Blend",
    "Animation.IK",
    "Animation.Retarget",
    "Animation.Morph",
    "Animation.Skinning",
    "Animation.Graph"
)
