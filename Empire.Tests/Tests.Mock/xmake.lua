target("Tests.Mock")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS")
