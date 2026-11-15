{
    depfiles_format = "gcc",
    depfiles = "Shm.o: Engine/Platform/Empire.Window/Wayland/Shm.cpp  Engine/Platform/Empire.Window/Wayland/Shm.hpp  Engine/Platform/Empire.Window/Wayland/../../Empire.App/Wayland/Interface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/Wayland/ShmPool.hpp  Engine/Platform/Empire.Window/Wayland/Buffer.hpp\
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
    },
    files = {
        "Engine/Platform/Empire.Window/Wayland/Shm.cpp"
    }
}