{
    depfiles_format = "gcc",
    depfiles = "SceneMain.o: Tests/Rendering/SceneMain.cpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/IWindowImpl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/NativeWindowHandle.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowConfig.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowBackend.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowImplFactory.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkDevice.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkCommon.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkInstance.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.SwapChain/Vulkan/VkSurface.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.SwapChain/Vulkan/VkSwapChain.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.CommandBuffer/Vulkan/VkFrameContext.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Buffer/Vulkan/VkMesh.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Buffer/Vulkan/../../Empire.Mesh/Mesh.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Buffer/Vulkan/VkBuffer.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Shader/Vulkan/VkShaderModule.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/VkSceneTarget.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/../../Empire.Texture/Vulkan/VkDepthImage.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/VkUniformSet.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/../../Empire.Texture/Vulkan/VkTexture2D.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/../../Empire.Texture/Vulkan/../NoiseImage.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Pipeline/Vulkan/VkGraphicsPipeline.hpp  Tests/Rendering/SceneMath.hpp\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++26",
            "-I/home/devchik10/Documents/Empire.Engine",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Rendering",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DEMPIRE_SHADER_DIR=\"/home/devchik10/Documents/Empire.Engine/build/shaders\"",
            "-DNDEBUG"
        }
    },
    files = {
        "Tests/Rendering/SceneMain.cpp"
    }
}