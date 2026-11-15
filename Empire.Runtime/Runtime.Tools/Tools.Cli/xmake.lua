target("Tools.Cli")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Application", "Engine.Assets")
