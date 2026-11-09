-- клиент и сервер в одном процессе (peer-to-peer, client-host); режим задаётся аргументами запуска
target("Runtime.Standalone")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Runtime.Shared", "Engine.Rendering", "Engine.Sound", "Engine.Animation", "Engine.UI", "Engine.Input", "Engine.Scripting")
