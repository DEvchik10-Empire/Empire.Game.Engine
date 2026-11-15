target("Graphics.Pipeline")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Shader", "Graphics.BindGroup")

if has_config("vulkan") then includes("Pipeline.Vulkan") end
if has_config("d3d12") then includes("Pipeline.DirectX12") end
if has_config("d3d11") then includes("Pipeline.DirectX11") end
if has_config("metal") then includes("Pipeline.Metal") end
if has_config("opengl") then includes("Pipeline.OpenGL") end
