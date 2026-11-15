target("Graphics.Query")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device")

if has_config("vulkan") then includes("Query.Vulkan") end
if has_config("d3d12") then includes("Query.DirectX12") end
if has_config("d3d11") then includes("Query.DirectX11") end
if has_config("metal") then includes("Query.Metal") end
if has_config("opengl") then includes("Query.OpenGL") end
