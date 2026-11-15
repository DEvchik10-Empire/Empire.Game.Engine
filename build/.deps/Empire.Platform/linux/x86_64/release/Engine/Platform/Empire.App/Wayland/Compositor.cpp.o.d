{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.App/Wayland/Compositor.cpp"
    },
    depfiles = "Compositor.o: Engine/Platform/Empire.App/Wayland/Compositor.cpp  Engine/Platform/Empire.App/Wayland/Compositor.hpp  Engine/Platform/Empire.App/Wayland/Interface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.App/Wayland/../../Empire.Window/Wayland/Surface.hpp\
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