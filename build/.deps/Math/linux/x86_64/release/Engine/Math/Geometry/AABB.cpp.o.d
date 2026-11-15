{
    depfiles_format = "gcc",
    depfiles = "AABB.o: Engine/Math/Geometry/AABB.cpp Engine/Math/Geometry/AABB.hpp  Engine/Math/Geometry/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
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
        "Engine/Math/Geometry/AABB.cpp"
    }
}