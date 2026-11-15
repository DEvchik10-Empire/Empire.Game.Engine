target("Engine.Rendering")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Engine.Assets", "Math.Geometry", "Math.Matrix", "Graphics.Device", "Graphics.Buffer", "Graphics.Texture", "Graphics.Pipeline", "Graphics.CommandBuffer", "Graphics.SwapChain")

includes(
    "Rendering.Resource",
    "Rendering.Frame",
    "Rendering.Shader",
    "Rendering.Graph",
    "Rendering.Mesh",
    "Rendering.Image",
    "Rendering.Material",
    "Rendering.Technique",
    "Rendering.View",
    "Rendering.Light",
    "Rendering.Shadow",
    "Rendering.Environment",
    "Rendering.PostProcess",
    "Rendering.Particle",
    "Rendering.Debug"
)
