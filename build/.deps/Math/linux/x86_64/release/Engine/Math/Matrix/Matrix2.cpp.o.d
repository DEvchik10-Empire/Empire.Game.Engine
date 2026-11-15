{
    depfiles = "Matrix2.o: Engine/Math/Matrix/Matrix2.cpp Engine/Math/Matrix/Matrix2.hpp  Engine/Math/Matrix/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector2.hpp  ../Empire.Engine/Engine/Math/Math.hpp\
",
    files = {
        "Engine/Math/Matrix/Matrix2.cpp"
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