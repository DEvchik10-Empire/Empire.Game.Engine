target("Graphics.SwapChain")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("SwapChain.DirectX11") end
if has_config("d3d12") then includes("SwapChain.DirectX12") end
if has_config("metal") then includes("SwapChain.Metal") end
if has_config("opengl") then includes("SwapChain.OpenGL") end
if has_config("vulkan") then includes("SwapChain.Vulkan") end
