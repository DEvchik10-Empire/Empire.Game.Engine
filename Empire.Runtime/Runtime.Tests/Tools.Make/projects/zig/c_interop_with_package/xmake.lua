add_rules("mode.debug", "mode.release")
add_requires("zlib", {system = false})

target("test")
    set_kind("binary")
    add_files("src/*.zig")
    add_packages("zlib")
