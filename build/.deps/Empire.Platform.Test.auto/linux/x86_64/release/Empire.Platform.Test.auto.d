{
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
    },
    files = {
        "build/linux/x86_64/release/libEmpire.Platform.a",
        "build/.objs/Empire.Platform.Test.auto/linux/x86_64/release/Engine/Platform/Tests/test.cpp.o"
    }
}