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
        "Quaternion/Quaternion.cpp"
    },
    depfiles = "Quaternion.o: Quaternion/Quaternion.cpp Quaternion/Quaternion.hpp  Quaternion/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    depfiles_format = "gcc"
}