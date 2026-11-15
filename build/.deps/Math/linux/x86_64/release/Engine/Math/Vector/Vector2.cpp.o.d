{
    depfiles = "Vector2.o: Engine/Math/Vector/Vector2.cpp Engine/Math/Vector/Vector2.hpp  Engine/Math/Vector/../../../Kernel/Types.hpp\
",
    files = {
        "Engine/Math/Vector/Vector2.cpp"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++26",
            "-I.",
            "-IEngine/Math",
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc"
}