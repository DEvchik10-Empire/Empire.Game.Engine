-- клиент: окно, рендер, звук, UI, сеть
target("Empire.Client")
    set_kind("binary")
    set_values("Empire.Layer", "Runtime")
    add_files("*.cpp")
    add_deps("Runtime.Shared", "Engine.Rendering", "Engine.Sound", "Engine.Animation", "Engine.UI", "Engine.Input", "Engine.Scripting")
