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
        "Geometry/OBB.cpp"
    },
    depfiles = "OBB.o: Geometry/OBB.cpp Geometry/OBB.hpp  Geometry/../../../Kernel/Types.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math/Quaternion/Quaternion.hpp  ../../../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    depfiles_format = "gcc"
}