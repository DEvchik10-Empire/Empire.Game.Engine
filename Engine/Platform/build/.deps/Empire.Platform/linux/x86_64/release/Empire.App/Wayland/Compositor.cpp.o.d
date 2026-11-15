{
    depfiles_format = "gcc",
    depfiles = "Compositor.o: Empire.App/Wayland/Compositor.cpp  Empire.App/Wayland/Compositor.hpp Empire.App/Wayland/Interface.hpp  ../../../Empire.Engine/Kernel/Types.hpp  Empire.App/Wayland/../../Empire.Window/Wayland/Surface.hpp\
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
        "Empire.App/Wayland/Compositor.cpp"
    }
}