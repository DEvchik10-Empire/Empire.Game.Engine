{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.App/Wayland/Display.cpp"
    },
    depfiles = "Display.o: Engine/Platform/Empire.App/Wayland/Display.cpp  Engine/Platform/Empire.App/Wayland/Display.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp\
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