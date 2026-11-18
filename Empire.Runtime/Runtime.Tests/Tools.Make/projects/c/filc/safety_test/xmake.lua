add_rules("mode.debug", "mode.release")

add_requires("filc")

target("safety_test")
    set_kind("binary")
    set_toolchains("@filc")
    add_packages("filc")
    add_files("src/*.c")
