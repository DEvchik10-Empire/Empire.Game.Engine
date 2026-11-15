-- клиент: окно, рендер, звук, UI, сеть
target("Runtime.Client")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Runtime.Shared", "Engine.Rendering", "Engine.Sound", "Engine.Animation", "Engine.UI", "Engine.Input", "Engine.Scripting")
