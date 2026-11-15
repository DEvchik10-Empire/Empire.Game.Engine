-- основная точка входа игры
target("Empire.Game")
    set_kind("binary")
    set_values("Empire.Layer", "Runtime")
    add_files("*.cpp")
    add_deps("Runtime.Shared", "Engine.Rendering", "Engine.Sound", "Engine.Animation", "Engine.UI", "Engine.Input", "Engine.Scripting")
