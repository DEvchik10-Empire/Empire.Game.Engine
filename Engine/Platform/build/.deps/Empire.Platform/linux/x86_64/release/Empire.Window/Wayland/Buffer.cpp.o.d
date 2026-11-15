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
        "Empire.Window/Wayland/Buffer.cpp"
    },
    depfiles = "Buffer.o: Empire.Window/Wayland/Buffer.cpp  Empire.Window/Wayland/Buffer.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/../../Kernel/Types.hpp\
"
}