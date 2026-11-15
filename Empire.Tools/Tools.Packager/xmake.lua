target("Tools.Packager")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Assets")
