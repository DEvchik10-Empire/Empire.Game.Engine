{
    depfiles = "CatmullRom.o: Engine/Math/Geometry/CatmullRom.cpp  Engine/Math/Geometry/CatmullRom.hpp  Engine/Math/Geometry/../../../Kernel/Types.hpp  ../Empire.Engine/Engine/Math/Vector/Vector3.hpp\
",
    files = {
        "Engine/Math/Geometry/CatmullRom.cpp"
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