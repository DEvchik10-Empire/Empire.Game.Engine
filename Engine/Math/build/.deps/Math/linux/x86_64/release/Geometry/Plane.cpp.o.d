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
        "Geometry/Plane.cpp"
    },
    depfiles = "Plane.o: Geometry/Plane.cpp Geometry/Plane.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    depfiles_format = "gcc"
}