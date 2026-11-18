target("Graphics.Fence")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Fence.DirectX11") end
if has_config("d3d12") then includes("Fence.DirectX12") end
if has_config("metal") then includes("Fence.Metal") end
if has_config("opengl") then includes("Fence.OpenGL") end
if has_config("vulkan") then includes("Fence.Vulkan") end
