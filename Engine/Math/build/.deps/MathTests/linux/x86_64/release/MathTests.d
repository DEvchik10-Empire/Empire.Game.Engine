{
    files = {
        "build/linux/x86_64/release/libMath.a",
        "build/.objs/MathTests/linux/x86_64/release/Tests/main.cpp.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-Lbuild/linux/x86_64/release",
            "-lMath"
        }
    }
}