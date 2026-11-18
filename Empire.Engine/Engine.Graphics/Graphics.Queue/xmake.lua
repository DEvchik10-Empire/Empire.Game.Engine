target("Graphics.Queue")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Queue.DirectX11") end
if has_config("d3d12") then includes("Queue.DirectX12") end
if has_config("metal") then includes("Queue.Metal") end
if has_config("opengl") then includes("Queue.OpenGL") end
if has_config("vulkan") then includes("Queue.Vulkan") end
