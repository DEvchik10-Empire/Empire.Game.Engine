set_languages("c++26")

target("Math")
    set_kind("static")

    add_files("Math.cpp")
    add_files("Transform/*.cpp")
    add_files("Geometry/*.cpp")
    add_files("Vector/*.cpp")
    add_files("Quaternion/*.cpp")
    add_files("Matrix/*.cpp")

    add_includedirs("../../", {public = true})
    add_includedirs(".", {public = true})

target("MathTests")
    set_kind("binary")

    add_files("Tests/main.cpp")

    add_includedirs(".", {public = true})

    add_deps("Math")