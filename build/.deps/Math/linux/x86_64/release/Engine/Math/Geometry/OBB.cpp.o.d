{
    depfiles_format = "gcc",
    depfiles = "OBB.o: Engine/Math/Geometry/OBB.cpp Engine/Math/Geometry/OBB.hpp  Engine/Math/Geometry/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../Empire.Engine/Engine/Math/Quaternion/Quaternion.hpp  ../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
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
        "Engine/Math/Geometry/OBB.cpp"
    }
}