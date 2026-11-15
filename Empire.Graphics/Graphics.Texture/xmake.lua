target("Graphics.Texture")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device")

if has_config("vulkan") then includes("Texture.Vulkan") end
if has_config("d3d12") then includes("Texture.DirectX12") end
if has_config("d3d11") then includes("Texture.DirectX11") end
if has_config("metal") then includes("Texture.Metal") end
if has_config("opengl") then includes("Texture.OpenGL") end
