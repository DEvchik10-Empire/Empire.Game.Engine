{
    depfiles = "Matrix4.o: Engine/Math/Matrix/Matrix4.cpp Engine/Math/Matrix/Matrix4.hpp  Engine/Math/Matrix/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    files = {
        "Engine/Math/Matrix/Matrix4.cpp"
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