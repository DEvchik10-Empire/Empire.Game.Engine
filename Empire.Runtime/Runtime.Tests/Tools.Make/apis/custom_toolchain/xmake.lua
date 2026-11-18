add_rules("mode.debug", "mode.release")

add_toolchaindirs("xmake/toolchains")

set_toolchains("my-c6000")

target("test")
    set_kind("static")
    add_files("src/foo.cpp")

target("demo")
    set_kind("binary")
    add_deps("test")
    add_files("src/test.cpp")


