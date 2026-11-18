includes(
    "Nodes.Kernel",
    "Nodes.Hierarchy",
    "Nodes.Prefab",
    "Nodes.Scene",
    "Nodes.Serialization",
    "Nodes.Types",
    "Nodes.Camera",
    "Nodes.2D",
    "Nodes.3D",
    "Nodes.UI",
    "Nodes.Anim",
    "Nodes.Phys",
    "Nodes.Nav",
    "Nodes.Render",
    "Nodes.Particle",
    "Nodes.Audio",
    "Nodes.Video",
    "Nodes.XR"
)

if has_config("editor") and not is_mode("shipping") then
    includes("Nodes.Editor")
end
