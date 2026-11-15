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
        "Geometry/Frustum.cpp"
    },
    depfiles = "Frustum.o: Geometry/Frustum.cpp Geometry/Frustum.hpp  Geometry/../../../Kernel/Types.hpp Geometry/Plane.hpp  ../../../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../../../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../../../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    depfiles_format = "gcc"
}