{
    depfiles_format = "gcc",
    depfiles = "Surface.o: Empire.Window/Wayland/Surface.cpp  Empire.Window/Wayland/Surface.hpp  ../../../Empire.Engine/Kernel/Types.hpp\
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
        "Empire.Window/Wayland/Surface.cpp"
    }
}