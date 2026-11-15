{
    depfiles_format = "gcc",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++23",
            "-I/home/devchik10/Documents/Empire.Engine",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DNDEBUG"
        }
    },
    files = {
        "Empire.Window/Wayland/Shm.cpp"
    },
    depfiles = "Shm.o: Empire.Window/Wayland/Shm.cpp Empire.Window/Wayland/Shm.hpp  Empire.Window/Wayland/../../Empire.App/Wayland/Interface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp  Empire.Window/Wayland/ShmPool.hpp Empire.Window/Wayland/Buffer.hpp\
"
}