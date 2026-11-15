target("Graphics.SwapChain")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.Assert", "Graphics.Common", "Graphics.ThirdParty", "Graphics.Device", "Graphics.Queue", "Platform.Surface")

if has_config("vulkan") then includes("SwapChain.Vulkan") end
if has_config("d3d12") then includes("SwapChain.DirectX12") end
if has_config("d3d11") then includes("SwapChain.DirectX11") end
if has_config("metal") then includes("SwapChain.Metal") end
if has_config("opengl") then includes("SwapChain.OpenGL") end
