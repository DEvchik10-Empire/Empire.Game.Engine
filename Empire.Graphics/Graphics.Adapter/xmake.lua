target("Graphics.Adapter")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Instance")

if has_config("vulkan") then includes("Adapter.Vulkan") end
if has_config("d3d12") then includes("Adapter.DirectX12") end
if has_config("d3d11") then includes("Adapter.DirectX11") end
if has_config("metal") then includes("Adapter.Metal") end
if has_config("opengl") then includes("Adapter.OpenGL") end
