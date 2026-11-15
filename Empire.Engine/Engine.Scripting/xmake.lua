target("Engine.Scripting")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Engine.ECS", "Engine.Node", "Engine.Gameplay", "Engine.Rendering", "Engine.Physics", "Engine.Sound", "Engine.Animation", "Engine.Networking", "Engine.UI", "Engine.AI", "Engine.Input")

includes(
    "Scripting.Runtime",
    "Scripting.Lua",
    "Scripting.Wasm",
    "Scripting.Bridge",
    "Scripting.Binding",
    "Scripting.Component",
    "Scripting.HotReload",
    "Scripting.Debugger"
)
