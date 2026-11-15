{
    depfiles_format = "gcc",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++23",
            "-I/home/devchik10/Documents/Empire.Engine",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DNDEBUG"
        }
    },
    files = {
        "Tests/Wayland/TestMain.cpp"
    },
    depfiles = "TestMain.o: Tests/Wayland/TestMain.cpp  Tests/Wayland/../../Empire.Window/Wayland/Impl.hpp  Tests/Wayland/../../Empire.Window/Wayland/WindowConfig.hpp  /home/devchik10/Documents/Empire.Engine/Engine/Platform/../../../Empire.Engine/Kernel/Types.hpp\
"
}