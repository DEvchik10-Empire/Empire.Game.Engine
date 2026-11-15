{
    depfiles = "Impl.o: Engine/Platform/Empire.Window/Wayland/Impl.cpp  Engine/Platform/Empire.Window/Wayland/Impl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/Wayland/../WindowConfig.hpp  Engine/Platform/Empire.Window/Wayland/../WindowBackend.hpp  Engine/Platform/Empire.Window/Wayland/../IWindowImpl.hpp  Engine/Platform/Empire.Window/Wayland/Window.hpp  Engine/Platform/Empire.Window/Wayland/Surface.hpp  Engine/Platform/Empire.Window/Wayland/XdgWmBase.hpp  /home/devchik10/Documents/Empire.Engine/../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h  /home/devchik10/Documents/Empire.Engine/Engine/Platform/Empire.App/Wayland/Interface.hpp  Engine/Platform/Empire.Window/Wayland/Shm.hpp  Engine/Platform/Empire.Window/Wayland/ShmPool.hpp  Engine/Platform/Empire.Window/Wayland/Buffer.hpp  Engine/Platform/Empire.Window/Wayland/../../Empire.App/Wayland/Compositor.hpp  Engine/Platform/Empire.Window/Wayland/../../Empire.App/Wayland/Display.hpp  Engine/Platform/Empire.Window/Wayland/../../Empire.App/Wayland/Registry.hpp\
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
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.Window/Wayland/Impl.cpp"
    }
}