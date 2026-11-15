{
    depfiles_format = "gcc",
    depfiles = "VkTexture2D.o: Engine/Rendering/Empire.Texture/Vulkan/VkTexture2D.cpp  Engine/Rendering/Empire.Texture/Vulkan/VkTexture2D.hpp  Engine/Rendering/Empire.Texture/Vulkan/../../Empire.Device/Vulkan/VkCommon.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Rendering/Empire.Texture/Vulkan/../../Empire.Device/Vulkan/VkDevice.hpp  Engine/Rendering/Empire.Texture/Vulkan/../../Empire.Device/Vulkan/VkInstance.hpp  Engine/Rendering/Empire.Texture/Vulkan/../NoiseImage.hpp  Engine/Rendering/Empire.Texture/Vulkan/../../Empire.Buffer/Vulkan/VkBuffer.hpp\
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
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Rendering",
            "-DNDEBUG"
        }
    },
    files = {
        "Engine/Rendering/Empire.Texture/Vulkan/VkTexture2D.cpp"
    }
}