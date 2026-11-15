{
    depfiles_format = "gcc",
    files = {
        "Engine/Rendering/Empire.SwapChain/Vulkan/VkSurface.cpp"
    },
    depfiles = "VkSurface.o: Engine/Rendering/Empire.SwapChain/Vulkan/VkSurface.cpp  Engine/Rendering/Empire.SwapChain/Vulkan/VkSurface.hpp  Engine/Rendering/Empire.SwapChain/Vulkan/../../Empire.Device/Vulkan/VkCommon.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Rendering/Empire.SwapChain/Vulkan/../../Empire.Device/Vulkan/VkInstance.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/NativeWindowHandle.hpp\
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
    }
}