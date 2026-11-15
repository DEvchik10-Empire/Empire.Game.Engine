{
    depfiles_format = "gcc",
    depfiles = "Vector3.o: Engine/Math/Vector/Vector3.cpp Engine/Math/Vector/Vector3.hpp  Engine/Math/Vector/../../../Kernel/Types.hpp\
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
        "Engine/Math/Vector/Vector3.cpp"
    }
}