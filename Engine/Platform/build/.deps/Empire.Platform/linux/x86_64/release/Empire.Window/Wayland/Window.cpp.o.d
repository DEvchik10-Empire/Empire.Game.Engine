{
    depfiles_format = "gcc",
    depfiles = "Window.o: Empire.Window/Wayland/Window.cpp  Empire.Window/Wayland/Window.hpp Empire.Window/Wayland/Surface.hpp  ../../../Empire.Engine/Kernel/Types.hpp  Empire.Window/Wayland/XdgWmBase.hpp  ../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h  ../Platform/Empire.App/Wayland/Interface.hpp  Empire.Window/Wayland/Shm.hpp Empire.Window/Wayland/ShmPool.hpp  Empire.Window/Wayland/Buffer.hpp  Empire.Window/Wayland/../../Empire.App/Wayland/Compositor.hpp\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++23",
            "-I../..",
            "-I.",
            "-IThirdParty/Wayland",
            "-DNDEBUG"
        }
    },
    files = {
        "Empire.Window/Wayland/Window.cpp"
    }
}