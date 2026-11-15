{
    depfiles_format = "gcc",
    depfiles = "main.o: Engine/Math/Tests/main.cpp Engine/Math/Vector/Vector3.hpp  Engine/Math/Vector/../../../Kernel/Types.hpp  Engine/Math/Matrix/Matrix4.hpp  ../Empire.Engine/Engine/Math//Vector/Vector4.hpp  Engine/Math/Quaternion/Quaternion.hpp Engine/Math/Math.hpp\
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
        "Engine/Math/Tests/main.cpp"
    }
}