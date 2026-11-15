{
    depfiles_format = "gcc",
    files = {
        "Engine/Rendering/Empire.Mesh/Mesh.cpp"
    },
    depfiles = "build/.objs/Empire.Rendering/linux/x86_64/release/Engine/Rendering/Empire.Mesh/Mesh.cpp.o:  Engine/Rendering/Empire.Mesh/Mesh.cpp  Engine/Rendering/Empire.Mesh/Mesh.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp\
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
            "-I/home/devchik10/Documents/Empire.Engine/Engine/Rendering",
            "-DNDEBUG"
        }
    }
}