{
    depfiles_format = "gcc",
    depfiles = "Display.o: Empire.App/Wayland/Display.cpp Empire.App/Wayland/Display.hpp  ../../../Empire.Engine/Kernel/Types.hpp\
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
        "Empire.App/Wayland/Display.cpp"
    }
}