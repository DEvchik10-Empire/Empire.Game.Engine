target("Empire.Tests.Benchmark")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Tests.Mock", "Engine.Application", "Engine.ECS")
