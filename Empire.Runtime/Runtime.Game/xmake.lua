-- основная точка входа игры
target("Runtime.Game")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Runtime.Shared", "Engine.Rendering", "Engine.Sound", "Engine.Animation", "Engine.UI", "Engine.Input", "Engine.Scripting")
    
includes(
    "Game.Client",
    "Game.Server",
    "Game.Standalone"
)

