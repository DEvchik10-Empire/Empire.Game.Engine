target("Graphics.CommandBuffer")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Pipeline", "Graphics.RenderPass", "Graphics.Buffer", "Graphics.Texture", "Graphics.Queue")

if has_config("vulkan") then includes("CommandBuffer.Vulkan") end
if has_config("d3d12") then includes("CommandBuffer.DirectX12") end
if has_config("d3d11") then includes("CommandBuffer.DirectX11") end
if has_config("metal") then includes("CommandBuffer.Metal") end
if has_config("opengl") then includes("CommandBuffer.OpenGL") end
