{
    depfiles = "Window.o: Engine/Platform/Empire.Window/legasy/Window.cpp  Engine/Platform/Empire.Window/legasy/Window.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/legasy/../WindowConfig.hpp  Engine/Platform/Empire.Window/legasy/../WindowBackend.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.App/legasy/Connection.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.App/legasy/EventQueue.hpp\
",
    files = {
        "Engine/Platform/Empire.Window/legasy/Window.cpp"
    },
    depfiles_format = "gcc",
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