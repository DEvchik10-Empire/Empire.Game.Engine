target("Graphics.Sampler")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device")

if has_config("vulkan") then includes("Sampler.Vulkan") end
if has_config("d3d12") then includes("Sampler.DirectX12") end
if has_config("d3d11") then includes("Sampler.DirectX11") end
if has_config("metal") then includes("Sampler.Metal") end
if has_config("opengl") then includes("Sampler.OpenGL") end
