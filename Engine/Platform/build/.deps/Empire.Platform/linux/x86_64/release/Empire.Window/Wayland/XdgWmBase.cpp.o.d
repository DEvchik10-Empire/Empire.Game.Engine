{
    depfiles_format = "gcc",
    depfiles = "XdgWmBase.o: Empire.Window/Wayland/XdgWmBase.cpp  Empire.Window/Wayland/XdgWmBase.hpp  ../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h  ../Platform/Empire.App/Wayland/Interface.hpp  ../../../Empire.Engine/Kernel/Types.hpp\
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
        "Empire.Window/Wayland/XdgWmBase.cpp"
    }
}