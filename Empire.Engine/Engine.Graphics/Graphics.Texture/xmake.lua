target("Graphics.Texture")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Texture.DirectX11") end
if has_config("d3d12") then includes("Texture.DirectX12") end
if has_config("metal") then includes("Texture.Metal") end
if has_config("opengl") then includes("Texture.OpenGL") end
if has_config("vulkan") then includes("Texture.Vulkan") end
