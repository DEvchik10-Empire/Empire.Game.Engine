target("Graphics.Instance")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Instance.DirectX11") end
if has_config("d3d12") then includes("Instance.DirectX12") end
if has_config("metal") then includes("Instance.Metal") end
if has_config("opengl") then includes("Instance.OpenGL") end
if has_config("vulkan") then includes("Instance.Vulkan") end
