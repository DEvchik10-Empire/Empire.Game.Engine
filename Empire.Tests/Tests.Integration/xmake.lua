target("Empire.Tests.Integration")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Tests.Mock", "Engine.Application", "Engine.ECS")
