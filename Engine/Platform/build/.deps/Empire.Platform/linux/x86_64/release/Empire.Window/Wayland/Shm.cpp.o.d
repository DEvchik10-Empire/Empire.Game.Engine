{
    depfiles_format = "gcc",
    depfiles = "Shm.o: Empire.Window/Wayland/Shm.cpp Empire.Window/Wayland/Shm.hpp  Empire.Window/Wayland/../../Empire.App/Wayland/Interface.hpp  ../../../Empire.Engine/Kernel/Types.hpp  Empire.Window/Wayland/ShmPool.hpp Empire.Window/Wayland/Buffer.hpp\
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
        "Empire.Window/Wayland/Shm.cpp"
    }
}