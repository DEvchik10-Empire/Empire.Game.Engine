target("Graphics.Shader")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Shader.DirectX11") end
if has_config("d3d12") then includes("Shader.DirectX12") end
if has_config("metal") then includes("Shader.Metal") end
if has_config("opengl") then includes("Shader.OpenGL") end
if has_config("vulkan") then includes("Shader.Vulkan") end
