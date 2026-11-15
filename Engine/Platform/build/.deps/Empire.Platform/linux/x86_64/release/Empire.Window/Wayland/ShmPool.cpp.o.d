{
    depfiles_format = "gcc",
    depfiles = "ShmPool.o: Empire.Window/Wayland/ShmPool.cpp  Empire.Window/Wayland/ShmPool.hpp Empire.Window/Wayland/Buffer.hpp  Empire.Window/Wayland/../../Empire.App/Wayland/Interface.hpp  ../../../Empire.Engine/Kernel/Types.hpp\
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
        "Empire.Window/Wayland/ShmPool.cpp"
    }
}