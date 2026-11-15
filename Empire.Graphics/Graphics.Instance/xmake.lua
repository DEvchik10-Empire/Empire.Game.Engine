target("Graphics.Instance")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty")

if has_config("vulkan") then includes("Instance.Vulkan") end
if has_config("d3d12") then includes("Instance.DirectX12") end
if has_config("d3d11") then includes("Instance.DirectX11") end
if has_config("metal") then includes("Instance.Metal") end
if has_config("opengl") then includes("Instance.OpenGL") end
