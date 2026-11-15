{
    depfiles = "WindowImplFactory.o: Engine/Platform/Empire.Window/WindowImplFactory.cpp  Engine/Platform/Empire.Window/WindowImplFactory.hpp  Engine/Platform/Empire.Window/IWindowImpl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/WindowConfig.hpp  Engine/Platform/Empire.Window/WindowBackend.hpp  Engine/Platform/Empire.Window/Wayland/Impl.hpp  Engine/Platform/Empire.Window/legasy/Impl.hpp\
",
    files = {
        "Engine/Platform/Empire.Window/WindowImplFactory.cpp"
    },
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
    depfiles_format = "gcc"
}