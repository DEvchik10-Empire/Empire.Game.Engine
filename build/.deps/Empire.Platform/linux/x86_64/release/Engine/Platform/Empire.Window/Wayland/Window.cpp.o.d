{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.Window/Wayland/Window.cpp"
    },
    depfiles = "Window.o: Engine/Platform/Empire.Window/Wayland/Window.cpp  Engine/Platform/Empire.Window/Wayland/Window.hpp  Engine/Platform/Empire.Window/Wayland/Surface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/Wayland/XdgWmBase.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.App/Wayland/Interface.hpp  Engine/Platform/Empire.Window/Wayland/Shm.hpp  Engine/Platform/Empire.Window/Wayland/ShmPool.hpp  Engine/Platform/Empire.Window/Wayland/Buffer.hpp  Engine/Platform/Empire.Window/Wayland/../../Empire.App/Wayland/Compositor.hpp\
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