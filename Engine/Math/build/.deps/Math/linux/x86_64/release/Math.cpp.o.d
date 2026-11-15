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
        "Math.cpp"
    },
    depfiles = "Math.o: Math.cpp Math.hpp ../../Kernel/Types.hpp\
",
    depfiles_format = "gcc"
}