{
    depfiles_format = "gcc",
    depfiles = "Interface.o: Empire.App/Wayland/Interface.cpp  Empire.App/Wayland/Interface.hpp ../../../Empire.Engine/Kernel/Types.hpp\
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
        "Empire.App/Wayland/Interface.cpp"
    }
}