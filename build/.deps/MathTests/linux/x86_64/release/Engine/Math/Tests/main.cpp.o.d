{
    depfiles = "main.o: Engine/Math/Tests/main.cpp Engine/Math/Vector/Vector3.hpp  Engine/Math/Vector/../../../Kernel/Types.hpp  Engine/Math/Matrix/Matrix4.hpp  ../Empire.Engine/Engine/Math//Vector/Vector4.hpp  Engine/Math/Quaternion/Quaternion.hpp Engine/Math/Math.hpp\
",
    files = {
        "Engine/Math/Tests/main.cpp"
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