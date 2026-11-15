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
        "Empire.Window/Wayland/XdgWmBase.cpp"
    },
    depfiles = "XdgWmBase.o: Empire.Window/Wayland/XdgWmBase.cpp  Empire.Window/Wayland/XdgWmBase.hpp  /home/devchik10/Documents/Empire.Engine/../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h  /home/devchik10/Documents/Empire.Engine/../Platform/Empire.App/Wayland/Interface.hpp  /home/devchik10/Documents/Empire.Engine/../Empire.Engine/Kernel/Types.hpp\
"
}