{
    depfiles_format = "gcc",
    depfiles = "Registry.o: Empire.App/Wayland/Registry.cpp  Empire.App/Wayland/Registry.hpp Empire.App/Wayland/Interface.hpp  ../../../Empire.Engine/Kernel/Types.hpp Empire.App/Wayland/Display.hpp\
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
        "Empire.App/Wayland/Registry.cpp"
    }
}