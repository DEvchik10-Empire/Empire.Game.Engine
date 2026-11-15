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
        "Empire.App/Wayland/Display.cpp"
    },
    depfiles = "Display.o: Empire.App/Wayland/Display.cpp Empire.App/Wayland/Display.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp\
"
}