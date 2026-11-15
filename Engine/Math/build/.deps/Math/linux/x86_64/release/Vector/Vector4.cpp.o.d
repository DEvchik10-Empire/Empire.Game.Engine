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
        "Vector/Vector4.cpp"
    },
    depfiles = "Vector4.o: Vector/Vector4.cpp Vector/Vector4.hpp  Vector/../../../Kernel/Types.hpp\
",
    depfiles_format = "gcc"
}