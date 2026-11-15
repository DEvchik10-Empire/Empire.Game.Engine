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
        "Geometry/Sphere.cpp"
    },
    depfiles = "Sphere.o: Geometry/Sphere.cpp Geometry/Sphere.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    depfiles_format = "gcc"
}