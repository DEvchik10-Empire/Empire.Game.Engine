{
    depfiles = "TestMain.o: Tests/Wayland/TestMain.cpp  Tests/Wayland/../../Empire.Window/Wayland/Impl.hpp  Tests/Wayland/../../Empire.Window/Wayland/WindowConfig.hpp  ../../../Empire.Engine/Kernel/Types.hpp\
",
    depfiles_format = "gcc",
    files = {
        "Tests/Wayland/TestMain.cpp"
    },
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
    }
}