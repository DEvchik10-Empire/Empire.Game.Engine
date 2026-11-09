target("Tools.ShaderCompiler")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Rendering")
