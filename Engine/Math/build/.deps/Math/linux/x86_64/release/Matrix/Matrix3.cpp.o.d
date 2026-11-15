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
        "Matrix/Matrix3.cpp"
    },
    depfiles = "Matrix3.o: Matrix/Matrix3.cpp Matrix/Matrix3.hpp  Matrix/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math/Math.hpp\
",
    depfiles_format = "gcc"
}