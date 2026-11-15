{
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-Lbuild/linux/x86_64/release",
            "-s",
            "-lMath"
        }
    },
    files = {
        "build/linux/x86_64/release/libMath.a",
        "build/.objs/MathTests/linux/x86_64/release/Engine/Math/Tests/main.cpp.o"
    }
}