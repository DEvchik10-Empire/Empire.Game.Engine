{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.Window/legasy/Impl.cpp"
    },
    depfiles = "Impl.o: Engine/Platform/Empire.Window/legasy/Impl.cpp  Engine/Platform/Empire.Window/legasy/Impl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Empire.Window/legasy/../WindowConfig.hpp  Engine/Platform/Empire.Window/legasy/../WindowBackend.hpp  Engine/Platform/Empire.Window/legasy/../IWindowImpl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/NativeWindowHandle.hpp  Engine/Platform/Empire.Window/legasy/Window.hpp  Engine/Platform/Empire.Window/legasy/../../Empire.App/legasy/Connection.hpp\
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
    }
}