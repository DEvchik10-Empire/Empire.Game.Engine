target("Math")
    set_kind("static")

    add_files("*.cpp")
    add_files("**/*.cpp")

    add_includedirs(".", {public = true})
    add_includedirs("..", {public = true})

    add_includedirs("Vector", {public = true})
    add_includedirs("Matrix", {public = true})
    add_includedirs("Quaternion", {public = true})
    add_includedirs("Transform", {public = true})

target("MathTests")
    set_kind("binary")

    add_files("Tests/main.cpp")

    add_deps("Math")