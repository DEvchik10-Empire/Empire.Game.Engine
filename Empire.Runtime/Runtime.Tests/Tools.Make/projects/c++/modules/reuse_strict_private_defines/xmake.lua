add_rules("mode.debug", "mode.release")

set_languages("c++20")

target("Producer")
    set_kind("static")
    add_defines("PRIVATE_DEP_DEFINE_DO_NOT_PROPAGATE")
    add_sysincludedirs("src/include", {public = true})
    add_files("src/mod.mpp", {public = true})
    add_files("src/mod.cpp")

target("Consumer")
    set_kind("binary")
    set_policy("build.c++.modules.reuse.strict", true)
    add_deps("Producer")
    add_files("src/main.cpp")
