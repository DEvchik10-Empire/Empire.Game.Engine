target("Graphics.Device")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Adapter")

if has_config("vulkan") then includes("Device.Vulkan") end
if has_config("d3d12") then includes("Device.DirectX12") end
if has_config("d3d11") then includes("Device.DirectX11") end
if has_config("metal") then includes("Device.Metal") end
if has_config("opengl") then includes("Device.OpenGL") end
