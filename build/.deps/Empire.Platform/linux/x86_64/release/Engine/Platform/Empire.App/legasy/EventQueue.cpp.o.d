{
    depfiles_format = "gcc",
    files = {
        "Engine/Platform/Empire.App/legasy/EventQueue.cpp"
    },
    depfiles = "EventQueue.o: Engine/Platform/Empire.App/legasy/EventQueue.cpp  Engine/Platform/Empire.App/legasy/EventQueue.hpp  Engine/Platform/Empire.App/legasy/Connection.hpp\
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