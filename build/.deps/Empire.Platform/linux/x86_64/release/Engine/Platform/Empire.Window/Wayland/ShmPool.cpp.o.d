{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.Window/Wayland/ShmPool.cpp"
    },
    depfiles = "ShmPool.o: Engine/Platform/Empire.Window/Wayland/ShmPool.cpp  Engine/Platform/Empire.Window/Wayland/ShmPool.hpp  Engine/Platform/Empire.Window/Wayland/Buffer.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp\
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
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DNDEBUG"
        }
    }
}