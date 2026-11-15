target("Graphics.Buffer")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device")

if has_config("vulkan") then includes("Buffer.Vulkan") end
if has_config("d3d12") then includes("Buffer.DirectX12") end
if has_config("d3d11") then includes("Buffer.DirectX11") end
if has_config("metal") then includes("Buffer.Metal") end
if has_config("opengl") then includes("Buffer.OpenGL") end
