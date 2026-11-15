{
    depfiles_format = "gcc",
    depfiles = "Registry.o: Engine/Platform/Empire.App/Wayland/Registry.cpp  Engine/Platform/Empire.App/Wayland/Registry.hpp  Engine/Platform/Empire.App/Wayland/Interface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.App/Wayland/Display.hpp\
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
        "Engine/Platform/Empire.App/Wayland/Registry.cpp"
    }
}