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
        "Matrix/Matrix4.cpp"
    },
    depfiles = "Matrix4.o: Matrix/Matrix4.cpp Matrix/Matrix4.hpp  Matrix/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    depfiles_format = "gcc"
}