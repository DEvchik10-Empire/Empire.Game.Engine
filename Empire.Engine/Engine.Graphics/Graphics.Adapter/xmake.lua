target("Graphics.Adapter")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Adapter.DirectX11") end
if has_config("d3d12") then includes("Adapter.DirectX12") end
if has_config("metal") then includes("Adapter.Metal") end
if has_config("opengl") then includes("Adapter.OpenGL") end
if has_config("vulkan") then includes("Adapter.Vulkan") end
