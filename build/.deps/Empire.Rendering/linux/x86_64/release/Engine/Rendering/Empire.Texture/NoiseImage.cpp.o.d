{
    depfiles_format = "gcc",
    depfiles = "NoiseImage.o: Engine/Rendering/Empire.Texture/NoiseImage.cpp  Engine/Rendering/Empire.Texture/NoiseImage.hpp  /home/devchik10/Documents/Empire.Engine/Kernel/Types.hpp\
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
    },
    files = {
        "Engine/Rendering/Empire.Texture/NoiseImage.cpp"
    }
}