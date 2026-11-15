{
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-std=c++26",
            "-I../..",
            "-I."
        }
    },
    files = {
        "Geometry/AABB.cpp"
    },
    depfiles = "AABB.o: Geometry/AABB.cpp Geometry/AABB.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    depfiles_format = "gcc"
}