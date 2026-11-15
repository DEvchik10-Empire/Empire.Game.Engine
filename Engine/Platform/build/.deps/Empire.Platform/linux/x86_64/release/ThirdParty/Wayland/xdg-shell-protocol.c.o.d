{
    depfiles_format = "gcc",
    depfiles = "xdg-shell-protocol.o: ThirdParty/Wayland/xdg-shell-protocol.c\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-O3",
            "-I../..",
            "-I.",
            "-IThirdParty/Wayland",
            "-DNDEBUG"
        }
    },
    files = {
        "ThirdParty/Wayland/xdg-shell-protocol.c"
    }
}