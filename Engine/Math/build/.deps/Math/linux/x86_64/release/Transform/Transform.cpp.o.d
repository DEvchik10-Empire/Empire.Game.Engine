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
        "Transform/Transform.cpp"
    },
    depfiles = "Transform.o: Transform/Transform.cpp Transform/Transform.hpp  Transform/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math//Quaternion/Quaternion.hpp  ../../../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    depfiles_format = "gcc"
}