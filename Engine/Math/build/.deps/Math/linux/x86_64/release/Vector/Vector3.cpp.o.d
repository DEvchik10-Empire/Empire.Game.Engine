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
        "Vector/Vector3.cpp"
    },
    depfiles = "Vector3.o: Vector/Vector3.cpp Vector/Vector3.hpp  Vector/../../../Kernel/Types.hpp\
",
    depfiles_format = "gcc"
}