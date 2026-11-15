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
        "Matrix/Matrix2.cpp"
    },
    depfiles = "Matrix2.o: Matrix/Matrix2.cpp Matrix/Matrix2.hpp  Matrix/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector2.hpp  ../../../Empire.Engine/Engine/Math/Math.hpp\
",
    depfiles_format = "gcc"
}