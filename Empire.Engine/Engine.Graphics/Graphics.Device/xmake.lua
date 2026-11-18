target("Graphics.Device")
    -- файлы добавляют бэкенды, см. includes ниже

if has_config("d3d11") then includes("Device.DirectX11") end
if has_config("d3d12") then includes("Device.DirectX12") end
if has_config("metal") then includes("Device.Metal") end
if has_config("opengl") then includes("Device.OpenGL") end
if has_config("vulkan") then includes("Device.Vulkan") end
