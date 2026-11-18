add_rules("mode.debug", "mode.release")

target("mathlib")
    set_kind("static")
    add_files("src/native/*.c")
    add_includedirs("src/native", {public = true})

target("test")
    set_kind("binary")
    add_deps("mathlib")
    add_files("src/*.zig")
    add_includedirs("src/native")
