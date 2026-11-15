{
    depfiles_format = "gcc",
    depfiles = "Math.o: Engine/Math/Math.cpp Engine/Math/Math.hpp  Engine/Math/../../Kernel/Types.hpp\
",
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
    files = {
        "Engine/Math/Math.cpp"
    }
}