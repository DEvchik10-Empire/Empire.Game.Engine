{
    depfiles = "TestMain.o: Tests/Rendering/TestMain.cpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/IWindowImpl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/NativeWindowHandle.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowConfig.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowBackend.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.Window/WindowImplFactory.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkDevice.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkCommon.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.Device/Vulkan/VkInstance.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.SwapChain/Vulkan/VkSurface.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.SwapChain/Vulkan/VkSwapChain.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Rendering/Empire.CommandBuffer/Vulkan/VkFrameContext.hpp\
",
    files = {
        "Tests/Rendering/TestMain.cpp"
    },
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
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc"
}