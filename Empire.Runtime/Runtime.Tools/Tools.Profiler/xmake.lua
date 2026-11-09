target("Tools.Profiler")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Diagnostics")
