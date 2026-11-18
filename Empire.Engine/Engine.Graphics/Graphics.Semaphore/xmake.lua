target("Graphics.Semaphore")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Semaphore.DirectX11") end
if has_config("d3d12") then includes("Semaphore.DirectX12") end
if has_config("metal") then includes("Semaphore.Metal") end
if has_config("opengl") then includes("Semaphore.OpenGL") end
if has_config("vulkan") then includes("Semaphore.Vulkan") end
