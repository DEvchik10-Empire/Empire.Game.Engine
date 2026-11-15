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
        "Geometry/CatmullRom.cpp"
    },
    depfiles = "CatmullRom.o: Geometry/CatmullRom.cpp Geometry/CatmullRom.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    depfiles_format = "gcc"
}