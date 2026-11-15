{
    files = {
        "build/linux/x86_64/release/libEmpire.Platform.a",
        "build/linux/x86_64/release/libEmpire.Rendering.a",
        "build/.objs/Empire.Rendering.Test.Scene/linux/x86_64/release/Tests/Rendering/SceneMain.cpp.o"
    },
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-Lbuild/linux/x86_64/release",
            "-s",
            "-lEmpire.Platform",
            "-lEmpire.Rendering",
            "-lvulkan",
            "-lwayland-client",
            "-lm",
            "-lxcb"
        }
    }
}