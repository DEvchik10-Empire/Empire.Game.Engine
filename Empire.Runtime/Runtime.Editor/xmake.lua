-- редактор: один исполняемый файл из подпапок Editor.*
target("Runtime.Editor")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("Engine.Application", "Engine.Assets", "Engine.ECS", "Engine.Node", "Engine.UI", "Engine.Input", "Engine.Rendering")

includes(
    "Editor.Framework",
    "Editor.Viewport",
    "Editor.Panel",
    "Editor.Gizmo",
    "Editor.Tool",
    "Editor.Plugin"
)
