target("Graphics.RenderPass")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("RenderPass.DirectX11") end
if has_config("d3d12") then includes("RenderPass.DirectX12") end
if has_config("metal") then includes("RenderPass.Metal") end
if has_config("opengl") then includes("RenderPass.OpenGL") end
if has_config("vulkan") then includes("RenderPass.Vulkan") end
