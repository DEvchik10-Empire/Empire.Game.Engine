{
    depfiles = "Transform.o: Engine/Math/Transform/Transform.cpp  Engine/Math/Transform/Transform.hpp  Engine/Math/Transform/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector3.hpp  ../Empire.Engine/Engine/Math//Quaternion/Quaternion.hpp  ../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp  ../Empire.Engine/Engine/Math//Vector/Vector4.hpp\
",
    files = {
        "Engine/Math/Transform/Transform.cpp"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++26",
            "-I.",
            "-IEngine/Math",
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc"
}