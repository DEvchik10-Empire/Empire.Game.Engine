{
    depfiles = "Vector4.o: Engine/Math/Vector/Vector4.cpp Engine/Math/Vector/Vector4.hpp  Engine/Math/Vector/../../../Kernel/Types.hpp\
",
    files = {
        "Engine/Math/Vector/Vector4.cpp"
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