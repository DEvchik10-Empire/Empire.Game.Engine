{
    depfiles = "TestMain.o: Engine/Platform/Tests/Wayland/TestMain.cpp  Engine/Platform/Tests/Wayland/../../Empire.Window/Wayland/Impl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Tests/Wayland/../../Empire.Window/Wayland/../WindowConfig.hpp  Engine/Platform/Tests/Wayland/../../Empire.Window/Wayland/../WindowBackend.hpp  Engine/Platform/Tests/Wayland/../../Empire.Window/Wayland/../IWindowImpl.hpp\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++26",
            "-I/home/devchik10/Documents/Empire.Engine",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform",
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Platform/ThirdParty/Wayland",
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Tests/Wayland/TestMain.cpp"
    }
}