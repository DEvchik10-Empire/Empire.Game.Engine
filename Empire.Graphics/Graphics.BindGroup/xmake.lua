target("Graphics.BindGroup")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Buffer", "Graphics.Texture", "Graphics.Sampler")

if has_config("vulkan") then includes("BindGroup.Vulkan") end
if has_config("d3d12") then includes("BindGroup.DirectX12") end
if has_config("d3d11") then includes("BindGroup.DirectX11") end
if has_config("metal") then includes("BindGroup.Metal") end
if has_config("opengl") then includes("BindGroup.OpenGL") end
