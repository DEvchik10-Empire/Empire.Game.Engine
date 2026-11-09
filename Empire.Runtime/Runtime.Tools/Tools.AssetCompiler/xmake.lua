target("Tools.AssetCompiler")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Assets")
