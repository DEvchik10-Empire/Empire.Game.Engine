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
        "Vector/Vector2.cpp"
    },
    depfiles = "Vector2.o: Vector/Vector2.cpp Vector/Vector2.hpp  Vector/../../../Kernel/Types.hpp\
",
    depfiles_format = "gcc"
}