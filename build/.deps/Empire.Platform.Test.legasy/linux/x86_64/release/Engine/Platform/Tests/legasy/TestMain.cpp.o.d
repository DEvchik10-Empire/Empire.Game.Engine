{
    depfiles = "TestMain.o: Engine/Platform/Tests/legasy/TestMain.cpp  Engine/Platform/Tests/legasy/../../Empire.Window/legasy/Impl.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp  Engine/Platform/Tests/legasy/../../Empire.Window/legasy/../WindowConfig.hpp  Engine/Platform/Tests/legasy/../../Empire.Window/legasy/../WindowBackend.hpp  Engine/Platform/Tests/legasy/../../Empire.Window/legasy/../IWindowImpl.hpp\
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
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Tests/legasy/TestMain.cpp"
    }
}