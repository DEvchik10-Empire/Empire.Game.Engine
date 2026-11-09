-- выделенный сервер без окна, рендера и звука
target("Runtime.Server")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Runtime.Shared")
