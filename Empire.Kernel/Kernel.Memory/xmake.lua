target("Kernel.Memory")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Types", "Kernel.System", "Kernel.Assert", "Kernel.Sync")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})
    -- Кэш потока аллокатора лежит в thread_local: initial-exec делает доступ к нему обычным обращением по fs
    -- вместо вызова __tls_get_addr (в разделяемой библиотеке разница около 2 раз). Kernel линкуется, а не dlopen-ится.
    add_cxxflags("-ftls-model=initial-exec", {tools = {"gcc", "clang"}})
