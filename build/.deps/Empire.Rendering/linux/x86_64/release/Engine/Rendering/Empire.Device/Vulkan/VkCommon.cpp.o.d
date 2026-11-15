{
    depfiles_format = "gcc",
    files = {
        "Engine/Rendering/Empire.Device/Vulkan/VkCommon.cpp"
    },
    depfiles = "VkCommon.o: Engine/Rendering/Empire.Device/Vulkan/VkCommon.cpp  Engine/Rendering/Empire.Device/Vulkan/VkCommon.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp\
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