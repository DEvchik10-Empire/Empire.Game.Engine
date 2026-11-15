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
        "Geometry/Bezier.cpp"
    },
    depfiles = "Bezier.o: Geometry/Bezier.cpp Geometry/Bezier.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    depfiles_format = "gcc"
}