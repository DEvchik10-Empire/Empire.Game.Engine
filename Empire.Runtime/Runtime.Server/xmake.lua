-- выделенный сервер без окна, рендера и звука
target("Empire.Server")
    set_kind("binary")
    set_values("Empire.Layer", "Runtime")
    add_files("*.cpp")
    add_deps("Runtime.Shared")
