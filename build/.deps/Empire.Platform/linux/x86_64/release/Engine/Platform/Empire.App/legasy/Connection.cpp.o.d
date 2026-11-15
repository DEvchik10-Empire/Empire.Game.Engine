{
    files = {
        "Engine/Platform/Empire.App/legasy/Connection.cpp"
    },
    depfiles_format = "gcc",
    depfiles = "Connection.o: Engine/Platform/Empire.App/legasy/Connection.cpp  Engine/Platform/Empire.App/legasy/Connection.hpp\
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