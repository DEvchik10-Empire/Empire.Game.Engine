{
    files = {
        "build/linux/x86_64/release/libEmpire.Platform.a",
        "build/.objs/Empire.Platform.Test.legasy/linux/x86_64/release/Engine/Platform/Tests/legasy/TestMain.cpp.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-Lbuild/linux/x86_64/release",
            "-s",
            "-lEmpire.Platform",
            "-lwayland-client",
            "-lm",
            "-lxcb"
        }
    }
}