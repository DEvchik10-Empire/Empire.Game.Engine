{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/ThirdParty/Wayland/xdg-shell-protocol.c"
    },
    depfiles = "xdg-shell-protocol.o:  Engine/Platform/ThirdParty/Wayland/xdg-shell-protocol.c\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-O3",
            "-I/home/devchik10/Documents/Empire.Engine",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DNDEBUG"
        }
    }
}