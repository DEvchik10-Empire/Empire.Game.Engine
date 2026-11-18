target("Graphics.BindGroup")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("BindGroup.DirectX11") end
if has_config("d3d12") then includes("BindGroup.DirectX12") end
if has_config("metal") then includes("BindGroup.Metal") end
if has_config("opengl") then includes("BindGroup.OpenGL") end
if has_config("vulkan") then includes("BindGroup.Vulkan") end
