target("Graphics.Fence")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device")

if has_config("vulkan") then includes("Fence.Vulkan") end
if has_config("d3d12") then includes("Fence.DirectX12") end
if has_config("d3d11") then includes("Fence.DirectX11") end
if has_config("metal") then includes("Fence.Metal") end
if has_config("opengl") then includes("Fence.OpenGL") end
