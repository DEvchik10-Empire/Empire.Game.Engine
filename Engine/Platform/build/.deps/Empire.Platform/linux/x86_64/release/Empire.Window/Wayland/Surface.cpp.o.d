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
        "Empire.Window/Wayland/Surface.cpp"
    },
    depfiles = "Surface.o: Empire.Window/Wayland/Surface.cpp  Empire.Window/Wayland/Surface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp\
"
}