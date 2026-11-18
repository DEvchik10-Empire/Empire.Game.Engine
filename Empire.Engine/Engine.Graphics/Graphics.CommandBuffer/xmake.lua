target("Graphics.CommandBuffer")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("CommandBuffer.DirectX11") end
if has_config("d3d12") then includes("CommandBuffer.DirectX12") end
if has_config("metal") then includes("CommandBuffer.Metal") end
if has_config("opengl") then includes("CommandBuffer.OpenGL") end
if has_config("vulkan") then includes("CommandBuffer.Vulkan") end
