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
        "Tests/main.cpp"
    },
    depfiles = "main.o: Tests/main.cpp Vector/Vector3.hpp  Vector/../../../Kernel/Types.hpp Matrix/Matrix4.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp  Quaternion/Quaternion.hpp Math.hpp\
",
    depfiles_format = "gcc"
}